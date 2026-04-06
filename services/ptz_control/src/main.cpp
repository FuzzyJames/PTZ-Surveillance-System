/// @file PTZ Control Service
/// Controls camera pan/tilt/zoom via ONVIF. Supports manual WASD/QE control,
/// automated patrol sweeps, and AI-driven person tracking.

#include "surveillance/config.h"
#include "surveillance/logging.h"
#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>

static std::atomic<bool> g_running{true};

void signal_handler(int signal) {
    g_running = false;
}

int main(int argc, char* argv[]) {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    surveillance::logging::init("ptz_control");
    LOG_INFO("PTZ Control Service starting...");
    LOG_INFO("Camera ONVIF endpoint: %s", surveillance::config::CAMERA_ONVIF_ENDPOINT);

    // TODO: Phase 3 implementation
    // 1. Initialize gSOAP ONVIF client
    // 2. Authenticate with camera (WS-Security UsernameToken)
    // 3. Get PTZ configuration (profiles, speed ranges, presets)
    // 4. Start gRPC server for PtzControlService
    // 5. Implement modes:
    //    a. MANUAL: Receive Move/Stop commands from GUI, translate to
    //       ONVIF ContinuousMove with appropriate velocity vectors
    //    b. PATROL: Slow sweep pattern:
    //       - Pan slowly left to limit, then right to limit
    //       - Tilt down to street level, then up slightly
    //       - Cover areas where people walk
    //    c. TRACK: Receive bounding box from AI service
    //       - Compute error (box center vs frame center)
    //       - PID-style control to keep person centered
    //       - Adjust zoom to maintain reasonable framing

    LOG_INFO("PTZ Control service running (stub). Waiting for shutdown signal...");
    while (g_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    LOG_INFO("PTZ Control Service stopped.");
    return 0;
}
