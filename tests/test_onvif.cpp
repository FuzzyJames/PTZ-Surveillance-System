/// @file ONVIF Client Test
/// Tests camera connectivity, service discovery, profile listing,
/// RTSP URI retrieval, and basic PTZ control.

#include "surveillance/onvif_client.h"
#include "surveillance/config.h"
#include "surveillance/logging.h"

#include <iostream>
#include <chrono>
#include <thread>

int main() {
    surveillance::logging::init("test_onvif");

    std::cout << "=== ONVIF Client Test ===" << std::endl;
    std::cout << "Camera endpoint: " << surveillance::config::CAMERA_ONVIF_ENDPOINT << std::endl;

    surveillance::OnvifClient client;
    client.configure(
        surveillance::config::CAMERA_ONVIF_ENDPOINT,
        surveillance::config::CAMERA_USERNAME,
        surveillance::config::CAMERA_PASSWORD
    );

    // Test 1: Ping
    std::cout << "\n--- Test 1: Ping (GetSystemDateAndTime) ---" << std::endl;
    auto start = std::chrono::steady_clock::now();
    bool ok = client.ping();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start).count();
    std::cout << "Ping: " << (ok ? "OK" : "FAILED") << " (" << elapsed << "ms)" << std::endl;
    if (!ok) {
        std::cerr << "Cannot reach camera. Aborting." << std::endl;
        return 1;
    }

    // Test 2: Discover services
    std::cout << "\n--- Test 2: Discover Services ---" << std::endl;
    start = std::chrono::steady_clock::now();
    ok = client.discoverServices();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start).count();
    std::cout << "Discovery: " << (ok ? "OK" : "FAILED") << " (" << elapsed << "ms)" << std::endl;
    std::cout << "  Media URL: " << client.mediaServiceUrl() << std::endl;
    std::cout << "  PTZ URL:   " << client.ptzServiceUrl() << std::endl;

    // Test 3: Get profiles
    std::cout << "\n--- Test 3: Get Media Profiles ---" << std::endl;
    start = std::chrono::steady_clock::now();
    auto profiles = client.getProfiles();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start).count();
    std::cout << "Profiles found: " << profiles.size() << " (" << elapsed << "ms)" << std::endl;
    for (const auto& p : profiles) {
        std::cout << "  " << p.token << ": " << p.width << "x" << p.height << std::endl;
    }

    if (profiles.empty()) {
        std::cerr << "No profiles found. Aborting." << std::endl;
        return 1;
    }

    // Test 4: Get RTSP URI
    std::cout << "\n--- Test 4: Get RTSP Stream URI ---" << std::endl;
    for (const auto& p : profiles) {
        start = std::chrono::steady_clock::now();
        std::string uri = client.getStreamUri(p.token);
        elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start).count();
        std::cout << "  " << p.token << " -> " << uri << " (" << elapsed << "ms)" << std::endl;
    }

    // Test 5: PTZ Get Position
    std::cout << "\n--- Test 5: PTZ Get Position ---" << std::endl;
    float pan = 0, tilt = 0, zoom = 0;
    start = std::chrono::steady_clock::now();
    ok = client.getPosition(profiles[0].token, pan, tilt, zoom);
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - start).count();
    if (ok) {
        std::cout << "  Pan=" << pan << " Tilt=" << tilt << " Zoom=" << zoom
                  << " (" << elapsed << "ms)" << std::endl;
    } else {
        std::cout << "  PTZ position query failed (" << elapsed << "ms)" << std::endl;
    }

    // Test 6: PTZ Move (small test - pan right briefly)
    std::cout << "\n--- Test 6: PTZ Move Test (pan right 0.5s) ---" << std::endl;
    ok = client.continuousMove(profiles[0].token, 0.3f, 0.0f, 0.0f);
    std::cout << "  ContinuousMove: " << (ok ? "OK" : "FAILED") << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ok = client.stopMove(profiles[0].token);
    std::cout << "  Stop: " << (ok ? "OK" : "FAILED") << std::endl;

    // Get new position
    ok = client.getPosition(profiles[0].token, pan, tilt, zoom);
    if (ok) {
        std::cout << "  New position: Pan=" << pan << " Tilt=" << tilt << " Zoom=" << zoom << std::endl;
    }

    std::cout << "\n=== All tests complete ===" << std::endl;

    // Log results to debug log
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    char ts[32];
    std::strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", std::localtime(&time_t_now));

    std::string logPath = "C:\\Users\\matta\\Documents\\C++ Repo\\AI Created Project\\Debugging Logs\\onvif_test.log";
    FILE* f = fopen(logPath.c_str(), "a");
    if (f) {
        fprintf(f, "[%s] ONVIF Test Results:\n", ts);
        fprintf(f, "  Ping: OK\n");
        fprintf(f, "  Profiles: %zu found\n", profiles.size());
        for (const auto& p : profiles) {
            fprintf(f, "    %s: %ux%u\n", p.token.c_str(), p.width, p.height);
        }
        fprintf(f, "  RTSP URI: %s\n", client.getStreamUri(profiles[0].token).c_str());
        fprintf(f, "  PTZ Move: tested\n\n");
        fclose(f);
        std::cout << "Results logged to: " << logPath << std::endl;
    }

    return 0;
}
