#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace surveillance {

/// Camera media profile information
struct MediaProfile {
    std::string token;
    std::string name;
    uint32_t width = 0;
    uint32_t height = 0;
    std::string video_encoding;  // "H264", "H265", etc.
};

/// PTZ configuration limits
struct PtzLimits {
    float pan_min = -1.0f, pan_max = 1.0f;
    float tilt_min = -1.0f, tilt_max = 1.0f;
    float zoom_min = 0.0f, zoom_max = 1.0f;
};

/// Lightweight ONVIF client using raw SOAP over HTTP.
/// No gSOAP dependency — uses WS-Security UsernameToken for auth.
class OnvifClient {
public:
    OnvifClient();
    ~OnvifClient();

    /// Set the device service endpoint and credentials
    void configure(const std::string& endpoint,
                   const std::string& username,
                   const std::string& password);

    /// Test connectivity (GetSystemDateAndTime — no auth needed)
    bool ping();

    /// Discover service URLs via GetCapabilities
    bool discoverServices();

    /// Get available media profiles
    std::vector<MediaProfile> getProfiles();

    /// Get the RTSP stream URI for a given profile token
    std::string getStreamUri(const std::string& profile_token);

    /// Get the RTSP stream URI for the main (first) profile
    std::string getMainStreamUri();

    // --- PTZ Control ---

    /// Continuous move (pan/tilt/zoom speeds from -1.0 to 1.0)
    bool continuousMove(const std::string& profile_token,
                        float pan_speed, float tilt_speed, float zoom_speed);

    /// Stop all PTZ movement
    bool stopMove(const std::string& profile_token);

    /// Absolute move to position
    bool absoluteMove(const std::string& profile_token,
                      float pan, float tilt, float zoom);

    /// Get current PTZ position
    bool getPosition(const std::string& profile_token,
                     float& pan, float& tilt, float& zoom);

    // --- Accessors ---
    const std::string& mediaServiceUrl() const { return m_mediaUrl; }
    const std::string& ptzServiceUrl() const { return m_ptzUrl; }
    const std::string& deviceServiceUrl() const { return m_deviceUrl; }

private:
    /// Send a SOAP request and return the response body
    std::string sendSoap(const std::string& url, const std::string& body,
                         bool use_auth = true);

    /// Build WS-Security UsernameToken XML header
    std::string buildWsSecurityHeader() const;

    /// Generate a SHA-1 password digest for WS-Security
    std::string computePasswordDigest(const std::string& nonce_b64,
                                       const std::string& created) const;

    /// Parse a simple XML value between tags
    static std::string extractXmlValue(const std::string& xml,
                                        const std::string& tag);

    /// Extract all occurrences of a value between tags
    static std::vector<std::string> extractAllXmlValues(const std::string& xml,
                                                         const std::string& tag);

    std::string m_deviceUrl;
    std::string m_mediaUrl;
    std::string m_ptzUrl;
    std::string m_imagingUrl;
    std::string m_analyticsUrl;
    std::string m_username;
    std::string m_password;
    int m_timeoutMs = 5000;
};

} // namespace surveillance
