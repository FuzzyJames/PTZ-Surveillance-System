#pragma once

#include <string>
#include <cstdint>

namespace surveillance {
namespace config {

// Camera settings
constexpr const char* CAMERA_ONVIF_ENDPOINT = "http://192.168.0.58:8080/onvif/device_service";
constexpr const char* CAMERA_USERNAME = "admin";
constexpr const char* CAMERA_PASSWORD = "password";

// Cluster service ports (gRPC)
constexpr uint16_t INGEST_SERVICE_PORT     = 50051;
constexpr uint16_t AI_DETECT_SERVICE_PORT  = 50052;
constexpr uint16_t PTZ_CONTROL_SERVICE_PORT = 50053;
constexpr uint16_t STORAGE_SERVICE_PORT    = 50054;
constexpr uint16_t STREAM_GATEWAY_PORT     = 50055;
constexpr uint16_t LOG_SERVICE_PORT        = 50056;

// Cluster node addresses
constexpr const char* NODE_INGEST   = "192.168.0.218";  // Pi1 - cluster1
constexpr const char* NODE_STORAGE  = "192.168.0.240";  // Pi2 - cluster2
constexpr const char* NODE_PTZ      = "192.168.0.90";   // Pi3 - cluster3
constexpr const char* NODE_STREAM   = "192.168.0.10";   // Pi4 - cluster4
constexpr const char* NODE_AI       = "192.168.0.207";  // Nano

// RTP multicast for video distribution
constexpr const char* RTP_MULTICAST_ADDR = "239.0.0.1";
constexpr uint16_t RTP_VIDEO_PORT        = 5004;
constexpr uint16_t RTP_AI_PORT           = 5006;

// Storage paths (on cluster)
constexpr const char* STORAGE_BASE_PATH  = "/mnt/wdpurple";
constexpr const char* VIDEO_PATH         = "/mnt/wdpurple/video";
constexpr const char* LOG_PATH           = "/mnt/wdpurple/logs";
constexpr const char* LOG_FILE           = "/mnt/wdpurple/logs/detection_log.txt";

// Storage limits
constexpr uint64_t MAX_LOG_SIZE_BYTES    = 1ULL * 1024 * 1024 * 1024;  // 1 GB

// Recording settings
constexpr uint32_t NORMAL_FPS            = 10;
constexpr uint32_t DETECTION_FPS         = 60;
constexpr uint32_t DETECTION_TAIL_SEC    = 30;   // seconds after person leaves
constexpr uint32_t DETECTION_PREBUF_SEC  = 30;   // seconds of pre-buffer
constexpr uint32_t HOURLY_SEGMENT_SEC    = 3600; // 1 hour per segment

// Streaming
constexpr uint16_t WEBRTC_SIGNAL_PORT    = 8080;

// Helper to build gRPC address strings
inline std::string grpc_address(const char* host, uint16_t port) {
    return std::string(host) + ":" + std::to_string(port);
}

} // namespace config
} // namespace surveillance
