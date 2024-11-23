#include "databasemanager.h"
#include <spdlog/spdlog.h>

DatabaseManager::DatabaseManager(const ConfigManager& config)
    : config_(config) {}

DatabaseManager::~DatabaseManager() {}

bool DatabaseManager::connect() {
    connectionPool_ = std::make_unique<ConnectionPool>(
        config_.getMySQLHost(), config_.getMySQLUser(),
        config_.getMySQLPassword(), config_.getMySQLDatabase(),
        config_.getMySQLPort(), 100, 200  // poolSize=100, maxSize=200
    );

    return isConnected();
}

bool DatabaseManager::isConnected() const {
    return connectionPool_ != nullptr;
}

MYSQL* DatabaseManager::getConnection(std::chrono::milliseconds timeout) {
    return connectionPool_->acquire(timeout);
}

void DatabaseManager::releaseConnection(MYSQL* conn) {
    connectionPool_->release(conn);
}
