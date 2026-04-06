#pragma once

#include <string>
#include <memory>

#ifdef HAS_SPDLOG
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#endif

namespace surveillance {
namespace logging {

/// Initialize the logging system.
/// @param service_name  Name of the service (used as logger name)
/// @param log_file      Optional file path for file logging
/// @param max_file_size Max log file size in bytes (default 10MB)
void init(const std::string& service_name,
          const std::string& log_file = "",
          size_t max_file_size = 10 * 1024 * 1024);

/// Log levels matching spdlog
enum class Level {
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Critical
};

/// Set the global log level
void set_level(Level level);

/// Get the service logger name (for use with spdlog::get())
const std::string& logger_name();

// Convenience macros (work with or without spdlog)
#ifdef HAS_SPDLOG
#define LOG_TRACE(...)    SPDLOG_TRACE(__VA_ARGS__)
#define LOG_DEBUG(...)    SPDLOG_DEBUG(__VA_ARGS__)
#define LOG_INFO(...)     SPDLOG_INFO(__VA_ARGS__)
#define LOG_WARN(...)     SPDLOG_WARN(__VA_ARGS__)
#define LOG_ERROR(...)    SPDLOG_ERROR(__VA_ARGS__)
#define LOG_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)
#else
#include <cstdio>
#define LOG_TRACE(...)    do { std::fprintf(stdout, "[TRACE] "); std::fprintf(stdout, __VA_ARGS__); std::fprintf(stdout, "\n"); } while(0)
#define LOG_DEBUG(...)    do { std::fprintf(stdout, "[DEBUG] "); std::fprintf(stdout, __VA_ARGS__); std::fprintf(stdout, "\n"); } while(0)
#define LOG_INFO(...)     do { std::fprintf(stdout, "[INFO]  "); std::fprintf(stdout, __VA_ARGS__); std::fprintf(stdout, "\n"); } while(0)
#define LOG_WARN(...)     do { std::fprintf(stderr, "[WARN]  "); std::fprintf(stderr, __VA_ARGS__); std::fprintf(stderr, "\n"); } while(0)
#define LOG_ERROR(...)    do { std::fprintf(stderr, "[ERROR] "); std::fprintf(stderr, __VA_ARGS__); std::fprintf(stderr, "\n"); } while(0)
#define LOG_CRITICAL(...) do { std::fprintf(stderr, "[CRIT]  "); std::fprintf(stderr, __VA_ARGS__); std::fprintf(stderr, "\n"); } while(0)
#endif

} // namespace logging
} // namespace surveillance
