// src/main.cpp
#include <boost/asio.hpp>
#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "config/logmanager.h"
#include "server/server.h"
#include "config/configmanager.h"
#include "database/databasemanager.h"
#include "monitoring/memorymonitor.h"

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

        // Inicializar o DatabaseManager e conectar ao MySQL
        DatabaseManager dbManager(configManager);
        if (!dbManager.connect()) {
            spdlog::error("Failed to connect to MySQL.");
            return 1;
        }

        boost::asio::io_context io_context;

        // Inicializar e iniciar o monitor de memória com base nas configurações
        double memoryThreshold = configManager.getMemoryThreshold();
        MemoryMonitor memoryMonitor(memoryThreshold, std::chrono::seconds(120));
        memoryMonitor.start();

        // Inicializar e iniciar o servidor com base nas configurações
        int serverPort = configManager.getServerPort();
        Server server(io_context, serverPort);

        // Executar o io_context na thread principal
        io_context.run();

        // Parar o monitor de memória quando o servidor parar
        memoryMonitor.stop();
    }
    catch (const std::exception& e) {
        spdlog::error("Exception: {}", e.what());
    }

    return 0;
}
