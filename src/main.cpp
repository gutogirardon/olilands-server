#include <boost/asio.hpp>
#include <string>
#include <thread>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "config/logmanager.h"
#include "server/server.h"
#include "config/configmanager.h"
#include "database/databasemanager.h"
#include "monitoring/memorymonitor.h"
#include "world/maploader.h"

int main() {
    try {
        // Inicializar o Gerenciador de Configurações
        ConfigManager configManager("data/config.lua");
        if (!configManager.loadConfig()) {
            spdlog::error("Failed to load config.lua.");
            return 1;
        }

        LogManager logManager;
        logManager.initialize(configManager.getLogLevel());

        // Inicializar o DatabaseManager com Pool de Conexões
        DatabaseManager dbManager(configManager);
        if (!dbManager.connect()) {
            spdlog::error("Failed to connect to MySQL.");
            return 1;
        }

        MapLoader mapLoader;
        if (!mapLoader.loadMap("data/map.tmx")) {
            spdlog::error("Failed to load the map.");
            return 1;
        }

        boost::asio::io_context io_context;

        // Inicializar e iniciar o monitor de memória com base nas configurações
        double memoryThreshold = configManager.getMemoryThreshold();
        MemoryMonitor memoryMonitor(memoryThreshold, std::chrono::seconds(120));
        memoryMonitor.start();

        // Inicializar e iniciar o servidor com base nas configurações
        int serverPort = configManager.getServerPort();
        Server server(io_context, serverPort, dbManager);

        // Criar um pool de threads para o io_context
        unsigned int threadCount = std::thread::hardware_concurrency();
        if (threadCount > 3) {
            threadCount = 3;
        }
        std::vector<std::thread> threads;
        spdlog::info("The number of threads used is {}", threadCount);

        for (unsigned int i = 0; i < threadCount; ++i) {
            threads.emplace_back([&io_context]() {
                io_context.run();
            });
        }

        // Aguardar que todas as threads terminem
        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }

        // Parar o monitor de memória quando o servidor parar
        memoryMonitor.stop();
    }
    catch (const std::exception& e) {
        spdlog::error("Exception: {}", e.what());
    }

    return 0;
}
