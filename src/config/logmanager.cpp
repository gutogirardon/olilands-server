#include "logmanager.h"
#include <spdlog/sinks/stdout_color_sinks.h>

void LogManager::initialize(const std::string& logLevel) {
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%s:%#] [%^%l%$] %v");

    if (logLevel == "debug") {
        spdlog::set_level(spdlog::level::debug);
    } else if (logLevel == "info") {
        spdlog::set_level(spdlog::level::info);
    } else if (logLevel == "warn") {
        spdlog::set_level(spdlog::level::warn);
    } else if (logLevel == "error") {
        spdlog::set_level(spdlog::level::err);
    } else {
        spdlog::set_level(spdlog::level::info);
        spdlog::warn("Unknown log level '{}'. Using 'info' as default.", logLevel);
    }
}
