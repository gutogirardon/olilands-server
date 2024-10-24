#include "databasemanager.h"
#include <spdlog/spdlog.h>

DatabaseManager::DatabaseManager(const ConfigManager& config)
    : config_(config) {}

DatabaseManager::~DatabaseManager() {}

bool DatabaseManager::connect() {
    connectionPool_ = std::make_unique<ConnectionPool>(
        config_.getMySQLHost(), config_.getMySQLUser(),
        config_.getMySQLPassword(), config_.getMySQLDatabase(),
        config_.getMySQLPort(), 10  // Tamanho do pool de conexões, por exemplo, 10
    );

    return true;  // Considerar que a conexão foi bem-sucedida
}

MYSQL* DatabaseManager::getConnection() {
    return connectionPool_->acquire();  // Adquirir conexão do pool
}

void DatabaseManager::releaseConnection(MYSQL* conn) {
    connectionPool_->release(conn);  // Devolver a conexão ao pool
}

bool DatabaseManager::isConnected() const {
    // Verifica se o pool tem conexões ativas
    return connectionPool_ != nullptr;
}
