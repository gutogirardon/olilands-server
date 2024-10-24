// src/main.cpp
#include <boost/asio.hpp>
#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "server/server.h"
#include "monitoring/memorymonitor.h"

int main() {
    try {
        // Configure spdlog
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v"); // Pattern with timestamp and colored log level
        spdlog::set_level(spdlog::level::info); // Set minimum log level to info

        boost::asio::io_context io_context;

        // Initialize and start the memory monitor
        MemoryMonitor memoryMonitor(80.0, std::chrono::seconds(5));
        memoryMonitor.start();

        // Initialize and start the server
        Server server(io_context, 7272);

        // Run the IO context in the main thread
        io_context.run();

        // Stop the memory monitor when the server stops
        memoryMonitor.stop();
    }
    catch (const std::exception& e) {
        spdlog::error("Exception: {}", e.what());
    }

    return 0;
}
