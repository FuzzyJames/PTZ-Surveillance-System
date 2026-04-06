/// @file Camera Ingest Service
/// Connects to PTZ camera via ONVIF, pulls RTSP stream,
/// and distributes H.264 frames via RTP multicast to other services.

#include "surveillance/config.h"
#include "surveillance/logging.h"
#include <iostream>
#include <csignal>
#include <atomic>

static std::atomic<bool> g_running{true};

void signal_handler(int signal) {
    std::cout << "Received signal " << signal << ", shutting down..." << std::endl;
    g_running = false;
}

int main(int argc, char* argv[]) {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    surveillance::logging::init("ingest");
    LOG_INFO("Camera Ingest Service starting...");
    LOG_INFO("ONVIF endpoint: %s", surveillance::config::CAMERA_ONVIF_ENDPOINT);

    // TODO: Phase 2 implementation
    // 1. Initialize GStreamer
    // 2. Discover camera RTSP URL via ONVIF (gSOAP)
    // 3. Build GStreamer pipeline:
    //    rtspsrc -> rtph264depay -> h264parse -> tee
    //      -> [queue -> RTP multicast to AI service (Nano)]
    //      -> [queue -> RTP multicast to Stream Gateway (Pi4)]
    //      -> [queue -> TCP to Storage Service (Pi2)]
    // 4. Start gRPC server for IngestService
    // 5. Monitor pipeline health, reconnect on failure

    LOG_INFO("Ingest service running (stub). Waiting for shutdown signal...");
    while (g_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    LOG_INFO("Camera Ingest Service stopped.");
    return 0;
}
