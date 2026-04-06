/// @file Log Service
/// Centralized detection log management. Receives detection events from AI,
/// writes to rolling log file (max 1GB), serves log queries and real-time
/// streaming to the Windows GUI.

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

    surveillance::logging::init("log_service");
    LOG_INFO("Log Service starting...");
    LOG_INFO("Log file path: %s", surveillance::config::LOG_FILE);
    LOG_INFO("Max log size: %llu bytes", surveillance::config::MAX_LOG_SIZE_BYTES);

    // TODO: Phase 4-5 implementation
    // 1. Open/create log file on WD Purple
    // 2. Start gRPC server for LogService
    // 3. WriteLog handler:
    //    a. Receive DetectionEvent from AI Service
    //    b. Format log line:
    //       [YYYY-MM-DD HH:MM:SS] VEHICLE | Type: SUV | Plate: ABC-1234 | Dir: NW | Owner: ...
    //       [YYYY-MM-DD HH:MM:SS] PERSON | Desc: Male, dark jacket | Video: detection_...mpg
    //    c. Append to log file
    //    d. Check file size, trim oldest lines if > 1GB
    //    e. Broadcast to all StreamLogs subscribers
    // 4. QueryLogs handler:
    //    a. Read log file with pagination (offset + limit)
    //    b. Support text search filtering
    //    c. Support date filtering
    // 5. StreamLogs handler:
    //    a. Server-side streaming of new log entries in real-time
    //    b. Client (GUI) connects and receives continuous updates

    LOG_INFO("Log service running (stub). Waiting for shutdown signal...");
    while (g_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    LOG_INFO("Log Service stopped.");
    return 0;
}
