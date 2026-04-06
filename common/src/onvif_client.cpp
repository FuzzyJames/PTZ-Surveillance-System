#include "surveillance/onvif_client.h"
#include "surveillance/logging.h"

#include <sstream>
#include <random>
#include <chrono>
#include <ctime>
#include <cstring>
#include <regex>
#include <algorithm>
#include <array>
#include <iomanip>

// Platform-specific HTTP
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincrypt.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "crypt32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif

namespace surveillance {

// ---------- Base64 encode/decode ----------

static const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static std::string base64_encode(const uint8_t* data, size_t len) {
    std::string result;
    result.reserve(((len + 2) / 3) * 4);
    for (size_t i = 0; i < len; i += 3) {
        uint32_t n = (uint32_t)data[i] << 16;
        if (i + 1 < len) n |= (uint32_t)data[i + 1] << 8;
        if (i + 2 < len) n |= (uint32_t)data[i + 2];
        result += b64_table[(n >> 18) & 0x3F];
        result += b64_table[(n >> 12) & 0x3F];
        result += (i + 1 < len) ? b64_table[(n >> 6) & 0x3F] : '=';
        result += (i + 2 < len) ? b64_table[n & 0x3F] : '=';
    }
    return result;
}

static std::vector<uint8_t> base64_decode(const std::string& input) {
    std::vector<uint8_t> result;
    std::array<int, 256> T{};
    T.fill(-1);
    for (int i = 0; i < 64; i++) T[(unsigned char)b64_table[i]] = i;
    uint32_t val = 0;
    int bits = -8;
    for (unsigned char c : input) {
        if (T[c] == -1) continue;
        val = (val << 6) + T[c];
        bits += 6;
        if (bits >= 0) {
            result.push_back((uint8_t)((val >> bits) & 0xFF));
            bits -= 8;
        }
    }
    return result;
}

// ---------- SHA-1 implementation ----------

static void sha1(const uint8_t* data, size_t len, uint8_t out[20]) {
#ifdef _WIN32
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    CryptAcquireContextW(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
    CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash);
    CryptHashData(hHash, data, (DWORD)len, 0);
    DWORD hashLen = 20;
    CryptGetHashParam(hHash, HP_HASHVAL, out, &hashLen, 0);
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
#else
    // Minimal SHA-1 for Linux (services will use OpenSSL in Docker)
    // For now, use a system call or link against libcrypto
    FILE* p = popen("echo -n '' | sha1sum", "r");
    if (p) { pclose(p); }
    // TODO: Replace with proper SHA-1 (OpenSSL or built-in)
    std::memset(out, 0, 20);
#endif
}

// ---------- HTTP request (platform-specific) ----------

#ifdef _WIN32
static std::string http_post(const std::string& url, const std::string& body,
                              const std::string& content_type, int timeout_ms) {
    // Parse URL
    std::wstring wurl(url.begin(), url.end());

    URL_COMPONENTS uc = {};
    uc.dwStructSize = sizeof(uc);
    wchar_t hostname[256] = {}, urlpath[1024] = {};
    uc.lpszHostName = hostname;
    uc.dwHostNameLength = 256;
    uc.lpszUrlPath = urlpath;
    uc.dwUrlPathLength = 1024;
    WinHttpCrackUrl(wurl.c_str(), 0, 0, &uc);

    HINTERNET hSession = WinHttpOpen(L"SurveillanceONVIF/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, NULL, NULL, 0);
    if (!hSession) return "";

    WinHttpSetTimeouts(hSession, timeout_ms, timeout_ms, timeout_ms, timeout_ms);

    HINTERNET hConnect = WinHttpConnect(hSession, hostname, uc.nPort, 0);
    if (!hConnect) { WinHttpCloseHandle(hSession); return ""; }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", urlpath,
        NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    std::wstring ct_header = L"Content-Type: " +
        std::wstring(content_type.begin(), content_type.end());
    WinHttpAddRequestHeaders(hRequest, ct_header.c_str(), (DWORD)-1,
        WINHTTP_ADDREQ_FLAG_REPLACE | WINHTTP_ADDREQ_FLAG_ADD);

    BOOL sent = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        (LPVOID)body.c_str(), (DWORD)body.size(), (DWORD)body.size(), 0);
    if (!sent) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    WinHttpReceiveResponse(hRequest, NULL);

    std::string response;
    DWORD bytesRead = 0;
    char buffer[4096];
    while (WinHttpReadData(hRequest, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        response.append(buffer, bytesRead);
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return response;
}
#else
// Linux: simple TCP socket-based HTTP POST
static std::string http_post(const std::string& url, const std::string& body,
                              const std::string& content_type, int timeout_ms) {
    // Parse URL: http://host:port/path
    std::regex url_re("http://([^:/]+)(?::(\\d+))?(/.*)?");
    std::smatch m;
    if (!std::regex_match(url, m, url_re)) return "";

    std::string host = m[1].str();
    int port = m[2].matched ? std::stoi(m[2].str()) : 80;
    std::string path = m[3].matched ? m[3].str() : "/";

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return "";

    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    struct hostent* he = gethostbyname(host.c_str());
    if (!he) { close(sock); return ""; }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    std::memcpy(&addr.sin_addr, he->h_addr, he->h_length);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sock);
        return "";
    }

    std::ostringstream req;
    req << "POST " << path << " HTTP/1.1\r\n"
        << "Host: " << host << ":" << port << "\r\n"
        << "Content-Type: " << content_type << "\r\n"
        << "Content-Length: " << body.size() << "\r\n"
        << "Connection: close\r\n\r\n"
        << body;

    std::string req_str = req.str();
    send(sock, req_str.c_str(), req_str.size(), 0);

    std::string response;
    char buf[4096];
    int n;
    while ((n = recv(sock, buf, sizeof(buf), 0)) > 0) {
        response.append(buf, n);
    }
    close(sock);

    // Strip HTTP headers
    auto hdr_end = response.find("\r\n\r\n");
    if (hdr_end != std::string::npos) {
        response = response.substr(hdr_end + 4);
    }
    return response;
}
#endif

// ---------- OnvifClient implementation ----------

OnvifClient::OnvifClient() = default;
OnvifClient::~OnvifClient() = default;

void OnvifClient::configure(const std::string& endpoint,
                            const std::string& username,
                            const std::string& password) {
    m_deviceUrl = endpoint;
    m_username = username;
    m_password = password;
    LOG_INFO("ONVIF client configured for %s", endpoint.c_str());
}

std::string OnvifClient::computePasswordDigest(const std::string& nonce_b64,
                                                const std::string& created) const {
    auto nonce_bytes = base64_decode(nonce_b64);
    std::vector<uint8_t> data;
    data.insert(data.end(), nonce_bytes.begin(), nonce_bytes.end());
    data.insert(data.end(), created.begin(), created.end());
    data.insert(data.end(), m_password.begin(), m_password.end());

    uint8_t hash[20];
    sha1(data.data(), data.size(), hash);
    return base64_encode(hash, 20);
}

std::string OnvifClient::buildWsSecurityHeader() const {
    // Generate 20-byte random nonce
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 255);
    uint8_t nonce_raw[20];
    for (auto& b : nonce_raw) b = (uint8_t)dist(gen);
    std::string nonce_b64 = base64_encode(nonce_raw, 20);

    // UTC timestamp
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    struct tm tm_buf;
#ifdef _WIN32
    gmtime_s(&tm_buf, &time_t_now);
#else
    gmtime_r(&time_t_now, &tm_buf);
#endif
    char created[32];
    std::strftime(created, sizeof(created), "%Y-%m-%dT%H:%M:%SZ", &tm_buf);

    std::string digest = computePasswordDigest(nonce_b64, created);

    std::ostringstream ss;
    ss << "<wsse:Security xmlns:wsse=\"http://docs.oasis-open.org/wss/2004/01/"
          "oasis-200401-wss-wssecurity-secext-1.0.xsd\" "
          "xmlns:wsu=\"http://docs.oasis-open.org/wss/2004/01/"
          "oasis-200401-wss-wssecurity-utility-1.0.xsd\">"
       << "<wsse:UsernameToken>"
       << "<wsse:Username>" << m_username << "</wsse:Username>"
       << "<wsse:Password Type=\"http://docs.oasis-open.org/wss/2004/01/"
          "oasis-200401-wss-username-token-profile-1.0#PasswordDigest\">"
       << digest << "</wsse:Password>"
       << "<wsse:Nonce EncodingType=\"http://docs.oasis-open.org/wss/2004/01/"
          "oasis-200401-wss-soap-message-security-1.0#Base64Binary\">"
       << nonce_b64 << "</wsse:Nonce>"
       << "<wsu:Created>" << created << "</wsu:Created>"
       << "</wsse:UsernameToken>"
       << "</wsse:Security>";
    return ss.str();
}

std::string OnvifClient::sendSoap(const std::string& url, const std::string& soap_body,
                                   bool use_auth) {
    std::ostringstream envelope;
    envelope << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
             << "<s:Envelope xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" "
                "xmlns:tds=\"http://www.onvif.org/ver10/device/wsdl\" "
                "xmlns:trt=\"http://www.onvif.org/ver10/media/wsdl\" "
                "xmlns:tt=\"http://www.onvif.org/ver10/schema\" "
                "xmlns:tptz=\"http://www.onvif.org/ver20/ptz/wsdl\">";

    if (use_auth) {
        envelope << "<s:Header>" << buildWsSecurityHeader() << "</s:Header>";
    }

    envelope << "<s:Body>" << soap_body << "</s:Body></s:Envelope>";

    std::string xml = envelope.str();
    return http_post(url, xml, "application/soap+xml; charset=utf-8", m_timeoutMs);
}

std::string OnvifClient::extractXmlValue(const std::string& xml, const std::string& tag) {
    // Handles namespaced tags like "tt:Uri" -> search for ":Uri>" or "<Uri>"
    std::string short_tag = tag;
    auto colon = tag.find(':');
    if (colon != std::string::npos) short_tag = tag.substr(colon + 1);

    // Search for <*:tag> or <tag>
    std::regex re("<[^>]*?" + short_tag + ">([^<]*)<");
    std::smatch m;
    if (std::regex_search(xml, m, re)) {
        return m[1].str();
    }
    return "";
}

std::vector<std::string> OnvifClient::extractAllXmlValues(const std::string& xml,
                                                           const std::string& tag) {
    std::vector<std::string> results;
    std::string short_tag = tag;
    auto colon = tag.find(':');
    if (colon != std::string::npos) short_tag = tag.substr(colon + 1);

    std::regex re("<[^>]*?" + short_tag + ">([^<]*)<");
    auto begin = std::sregex_iterator(xml.begin(), xml.end(), re);
    auto end = std::sregex_iterator();
    for (auto it = begin; it != end; ++it) {
        results.push_back((*it)[1].str());
    }
    return results;
}

bool OnvifClient::ping() {
    std::string response = sendSoap(m_deviceUrl,
        "<tds:GetSystemDateAndTime/>", false);
    return !response.empty() && response.find("GetSystemDateAndTimeResponse") != std::string::npos;
}

bool OnvifClient::discoverServices() {
    std::string response = sendSoap(m_deviceUrl,
        "<tds:GetCapabilities><tds:Category>All</tds:Category></tds:GetCapabilities>");

    if (response.empty()) {
        LOG_ERROR("GetCapabilities failed — empty response");
        return false;
    }

    // Extract service URLs from XAddr elements
    // The response has sections like <tt:Media><tt:XAddr>...</tt:XAddr></tt:Media>
    auto extract_service_url = [&](const std::string& section) -> std::string {
        auto pos = response.find(section);
        if (pos == std::string::npos) return "";
        auto sub = response.substr(pos, 500);
        return extractXmlValue(sub, "tt:XAddr");
    };

    m_mediaUrl = extract_service_url("<tt:Media>");
    m_ptzUrl = extract_service_url("<tt:PTZ>");
    m_imagingUrl = extract_service_url("<tt:Imaging>");
    m_analyticsUrl = extract_service_url("<tt:Analytics>");

    LOG_INFO("ONVIF services discovered:");
    LOG_INFO("  Media:     %s", m_mediaUrl.c_str());
    LOG_INFO("  PTZ:       %s", m_ptzUrl.c_str());
    LOG_INFO("  Imaging:   %s", m_imagingUrl.c_str());
    LOG_INFO("  Analytics: %s", m_analyticsUrl.c_str());

    return !m_mediaUrl.empty();
}

std::vector<MediaProfile> OnvifClient::getProfiles() {
    std::vector<MediaProfile> profiles;

    std::string response = sendSoap(m_mediaUrl, "<trt:GetProfiles/>");
    if (response.empty()) return profiles;

    // Find all profile tokens
    std::regex profile_re("token=\"([^\"]+)\"[^>]*fixed");
    // Actually parse profiles by finding <trt:Profiles token="..." ...>
    std::regex token_re("<trt:Profiles[^>]+token=\"([^\"]+)\"");
    auto begin = std::sregex_iterator(response.begin(), response.end(), token_re);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it) {
        MediaProfile p;
        p.token = (*it)[1].str();

        // Extract resolution for this profile (find next Width/Height after this token)
        auto pos = response.find(p.token);
        auto sub = response.substr(pos, 2000);

        auto w = extractXmlValue(sub, "tt:Width");
        auto h = extractXmlValue(sub, "tt:Height");
        if (!w.empty()) p.width = std::stoi(w);
        if (!h.empty()) p.height = std::stoi(h);

        profiles.push_back(p);
        LOG_INFO("  Profile: %s (%ux%u)", p.token.c_str(), p.width, p.height);
    }

    return profiles;
}

std::string OnvifClient::getStreamUri(const std::string& profile_token) {
    std::ostringstream body;
    body << "<trt:GetStreamUri>"
         << "<trt:StreamSetup>"
         << "<tt:Stream>RTP-Unicast</tt:Stream>"
         << "<tt:Transport><tt:Protocol>RTSP</tt:Protocol></tt:Transport>"
         << "</trt:StreamSetup>"
         << "<trt:ProfileToken>" << profile_token << "</trt:ProfileToken>"
         << "</trt:GetStreamUri>";

    std::string response = sendSoap(m_mediaUrl, body.str());
    // Try standard extraction first, then fallback patterns for Hikvision
    std::string uri = extractXmlValue(response, "tt:Uri");
    if (uri.empty()) {
        // Hikvision sometimes uses just <Uri> without namespace
        std::regex uri_re("<[^>]*Uri>([^<]+)</");
        std::smatch m;
        if (std::regex_search(response, m, uri_re)) {
            uri = m[1].str();
        }
    }
    if (uri.empty()) {
        // Last resort: look for rtsp:// anywhere in the response
        std::regex rtsp_re("(rtsp://[^<\"\\s]+)");
        std::smatch m;
        if (std::regex_search(response, m, rtsp_re)) {
            uri = m[1].str();
        }
    }
    LOG_INFO("Stream URI for %s: %s", profile_token.c_str(), uri.c_str());
    return uri;
}

std::string OnvifClient::getMainStreamUri() {
    auto profiles = getProfiles();
    if (profiles.empty()) {
        LOG_ERROR("No profiles found");
        return "";
    }
    return getStreamUri(profiles[0].token);
}

bool OnvifClient::continuousMove(const std::string& profile_token,
                                  float pan_speed, float tilt_speed, float zoom_speed) {
    std::ostringstream body;
    body << std::fixed << std::setprecision(2);
    body << "<tptz:ContinuousMove>"
         << "<tptz:ProfileToken>" << profile_token << "</tptz:ProfileToken>"
         << "<tptz:Velocity>"
         << "<tt:PanTilt x=\"" << pan_speed << "\" y=\"" << tilt_speed << "\"/>"
         << "<tt:Zoom x=\"" << zoom_speed << "\"/>"
         << "</tptz:Velocity>"
         << "</tptz:ContinuousMove>";

    std::string response = sendSoap(m_ptzUrl, body.str());
    return !response.empty() && response.find("Fault") == std::string::npos;
}

bool OnvifClient::stopMove(const std::string& profile_token) {
    std::ostringstream body;
    body << "<tptz:Stop>"
         << "<tptz:ProfileToken>" << profile_token << "</tptz:ProfileToken>"
         << "<tptz:PanTilt>true</tptz:PanTilt>"
         << "<tptz:Zoom>true</tptz:Zoom>"
         << "</tptz:Stop>";

    std::string response = sendSoap(m_ptzUrl, body.str());
    return !response.empty() && response.find("Fault") == std::string::npos;
}

bool OnvifClient::absoluteMove(const std::string& profile_token,
                                float pan, float tilt, float zoom) {
    std::ostringstream body;
    body << std::fixed << std::setprecision(4);
    body << "<tptz:AbsoluteMove>"
         << "<tptz:ProfileToken>" << profile_token << "</tptz:ProfileToken>"
         << "<tptz:Position>"
         << "<tt:PanTilt x=\"" << pan << "\" y=\"" << tilt << "\"/>"
         << "<tt:Zoom x=\"" << zoom << "\"/>"
         << "</tptz:Position>"
         << "</tptz:AbsoluteMove>";

    std::string response = sendSoap(m_ptzUrl, body.str());
    return !response.empty() && response.find("Fault") == std::string::npos;
}

bool OnvifClient::getPosition(const std::string& profile_token,
                               float& pan, float& tilt, float& zoom) {
    std::ostringstream body;
    body << "<tptz:GetStatus>"
         << "<tptz:ProfileToken>" << profile_token << "</tptz:ProfileToken>"
         << "</tptz:GetStatus>";

    std::string response = sendSoap(m_ptzUrl, body.str());
    if (response.empty()) return false;

    // Extract PanTilt position
    std::regex pt_re("PanTilt[^>]+x=\"([^\"]+)\"[^>]+y=\"([^\"]+)\"");
    std::smatch m;
    if (std::regex_search(response, m, pt_re)) {
        pan = std::stof(m[1].str());
        tilt = std::stof(m[2].str());
    }

    std::regex zoom_re("Zoom[^>]+x=\"([^\"]+)\"");
    if (std::regex_search(response, m, zoom_re)) {
        zoom = std::stof(m[1].str());
    }

    return true;
}

} // namespace surveillance
