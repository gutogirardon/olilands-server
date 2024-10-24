#pragma once
#include <spdlog/spdlog.h>
#include <string>

class LogManager {
public:
    void initialize(const std::string& logLevel);
};
