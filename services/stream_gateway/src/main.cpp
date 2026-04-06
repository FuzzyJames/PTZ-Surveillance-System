/// @file Streaming Gateway Service
/// Low-latency video relay from cluster to Windows GUI.
/// Receives RTP multicast from Ingest, forwards via WebRTC or raw RTP/UDP.
/// Also serves recorded video playback and clip export requests.

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

    surveillance::logging::init("stream_gateway");
    LOG_INFO("Streaming Gateway Service starting...");

    // TODO: Phase 2 implementation
    // 1. Join RTP multicast group to receive H.264 from Ingest
    // 2. Initialize WebRTC signaling server (HTTP on port 8080)
    //    - Accept SDP offers from GUI clients
    //    - Create WebRTC peer connections
    //    - Forward H.264 RTP packets directly (no transcode)
    // 3. Fallback: direct RTP/UDP forwarding for LAN clients
    // 4. Latency measurement:
    //    - Embed timestamps in RTP extension headers
    //    - Measure camera->cluster and cluster->GUI latency
    //    - Report via gRPC GetLatencyMetrics
    // 5. Playback proxy:
    //    - Receive playback requests from GUI via gRPC
    //    - Read MPEG files from NFS-mounted WD Purple
    //    - Stream back via gRPC server streaming
    // 6. Start gRPC server for StreamGatewayService

    LOG_INFO("Stream Gateway service running (stub). Waiting for shutdown signal...");
    while (g_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    LOG_INFO("Streaming Gateway Service stopped.");
    return 0;
}
