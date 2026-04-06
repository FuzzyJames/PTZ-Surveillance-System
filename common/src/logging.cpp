#include "surveillance/logging.h"
#include <iostream>

namespace surveillance {
namespace logging {

static std::string s_logger_name = "surveillance";

void init(const std::string& service_name,
          const std::string& log_file,
          size_t max_file_size) {
    s_logger_name = service_name;

#ifdef HAS_SPDLOG
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");

    std::vector<spdlog::sink_ptr> sinks = { console_sink };

    if (!log_file.empty()) {
        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            log_file, max_file_size, 3);
        file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
        sinks.push_back(file_sink);
    }

    auto logger = std::make_shared<spdlog::logger>(service_name, sinks.begin(), sinks.end());
    logger->set_level(spdlog::level::info);
    logger->flush_on(spdlog::level::warn);
    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);
#else
    std::cout << "[" << service_name << "] Logging initialized (fallback mode)" << std::endl;
    if (!log_file.empty()) {
        std::cout << "[" << service_name << "] File logging requested: " << log_file
                  << " (not available without spdlog)" << std::endl;
    }
#endif
}

void set_level(Level level) {
#ifdef HAS_SPDLOG
    switch (level) {
        case Level::Trace:    spdlog::set_level(spdlog::level::trace); break;
        case Level::Debug:    spdlog::set_level(spdlog::level::debug); break;
        case Level::Info:     spdlog::set_level(spdlog::level::info); break;
        case Level::Warn:     spdlog::set_level(spdlog::level::warn); break;
        case Level::Error:    spdlog::set_level(spdlog::level::err); break;
        case Level::Critical: spdlog::set_level(spdlog::level::critical); break;
    }
#else
    (void)level;
#endif
}

const std::string& logger_name() {
    return s_logger_name;
}

} // namespace logging
} // namespace surveillance
