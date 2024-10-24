// src/main.cpp
#include <boost/asio.hpp>
#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "server/server.h"
#include "monitoring/memorymonitor.h"
#include "config/configmanager.h"

int main() {
    try {
        // Inicializar o Gerenciador de Configurações
        ConfigManager configManager("data/config.lua");
        if (!configManager.loadConfig()) {
            spdlog::error("Falha ao carregar as configurações. Encerrando o servidor.");
            return 1;
        }

        // Configurar spdlog com base nas configurações
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v"); // Padrão com timestamp e nível de log colorido

        std::string logLevel = configManager.getLogLevel();
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
            spdlog::warn("Nível de log desconhecido '{}'. Usando 'info' por padrão.", logLevel);
        }

        boost::asio::io_context io_context;

        // Inicializar e iniciar o monitor de memória com base nas configurações
        double memoryThreshold = configManager.getMemoryThreshold();
        MemoryMonitor memoryMonitor(memoryThreshold, std::chrono::seconds(5));
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
