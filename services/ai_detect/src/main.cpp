/// @file AI Detection Service
/// Runs on Jetson Orin Nano. Receives video frames, performs object detection
/// (vehicles, people, license plates) using YOLOv8 + TensorRT, and publishes
/// detection events to other services via gRPC.

#include "surveillance/config.h"
#include "surveillance/logging.h"
#include <iostream>
#include <csignal>
#include <atomic>
#include <thread>

static std::atomic<bool> g_running{true};

void signal_handler(int signal) {
    std::cout << "Received signal " << signal << ", shutting down..." << std::endl;
    g_running = false;
}

int main(int argc, char* argv[]) {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    surveillance::logging::init("ai_detect");
    LOG_INFO("AI Detection Service starting on Jetson Orin Nano...");

    // TODO: Phase 5 implementation
    // 1. Load YOLOv8 TensorRT engine (or build from ONNX on first run)
    // 2. Initialize NVDEC for H.264 decode
    // 3. Connect to RTP multicast from Ingest Service
    // 4. Detection loop:
    //    a. Decode frame via NVDEC
    //    b. Run YOLOv8 inference (person + vehicle detection)
    //    c. For vehicles: crop plate region, run Tesseract OCR
    //    d. Classify vehicle type (car, truck, SUV, etc.)
    //    e. Estimate travel direction from frame-to-frame tracking
    //    f. Publish DetectionEvent via gRPC to Log Service
    //    g. If person detected:
    //       - Signal PTZ Control to switch to TRACK mode
    //       - Signal Storage to switch to 60fps DETECTION recording
    //       - Send bounding box to PTZ Control for tracking
    //    h. If no person (and was tracking):
    //       - Signal PTZ Control to resume PATROL mode
    //       - Signal Storage to switch back to 10fps after 30s tail
    // 5. Send annotated frames (with bounding boxes) to Stream Gateway
    // 6. Start gRPC server for AiDetectionService

    LOG_INFO("AI Detection service running (stub). Waiting for shutdown signal...");
    while (g_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    LOG_INFO("AI Detection Service stopped.");
    return 0;
}
