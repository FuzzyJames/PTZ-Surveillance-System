/// @file Storage Service
/// Manages video recording and storage on the WD Purple drive.
/// Handles folder hierarchy (Year/Month/Day), hourly segments at 10fps,
/// detection clips at 60fps with 30s pre-buffer, drive capacity management,
/// and rolling log file maintenance.

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

    surveillance::logging::init("storage");
    LOG_INFO("Storage Service starting...");
    LOG_INFO("Storage base path: %s", surveillance::config::STORAGE_BASE_PATH);

    // TODO: Phase 4 implementation
    // 1. Verify WD Purple mount at /mnt/wdpurple
    // 2. Create/verify folder structure: video/YYYY/MM/DD/
    // 3. Initialize FFmpeg muxer for MPEG output
    // 4. Start gRPC server for StorageService
    // 5. Recording pipeline:
    //    a. Receive H.264 stream from Ingest Service
    //    b. Mux into 1-hour MPEG segments at 10fps (normal mode)
    //    c. Maintain 30-second circular buffer of frames
    //    d. On DETECTION mode signal from AI:
    //       - Flush pre-buffer (30s at lower rate) to detection file
    //       - Switch to 60fps recording into Detections/ folder
    //       - Continue until 30s after person leaves frame
    //       - Name file with human-readable timestamp + description
    //    e. On return to NORMAL mode: resume 10fps hourly segments
    // 6. Drive management (runs as background task):
    //    a. At midnight: create new YYYY/MM/DD folder
    //    b. Check drive capacity periodically
    //    c. When full: delete oldest day folder, log the deletion
    //    d. Never delete current day's data
    // 7. Log management:
    //    a. Monitor log file size (max 1GB)
    //    b. When approaching limit: trim oldest lines
    // 8. Serve playback requests: list files, stream recordings, export clips

    LOG_INFO("Storage service running (stub). Waiting for shutdown signal...");
    while (g_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    LOG_INFO("Storage Service stopped.");
    return 0;
}
