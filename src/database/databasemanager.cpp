#include "databasemanager.h"
#include <spdlog/spdlog.h>

DatabaseManager::DatabaseManager(const ConfigManager& config)
    : config_(config), connection_(nullptr) {}

DatabaseManager::~DatabaseManager() {
    if (connection_) {
        mysql_close(connection_);
    }
}

bool DatabaseManager::connect() {
    connection_ = mysql_init(nullptr);
    if (connection_ == nullptr) {
        spdlog::error("MySQL initialization failed.");
        return false;
    }

    // @todo validar se no futuro caching_sha2_password é mais performartico!
    mysql_options(connection_, MYSQL_DEFAULT_AUTH, "mysql_native_password");

    if (!mysql_real_connect(connection_,
                            config_.getMySQLHost().c_str(),
                            config_.getMySQLUser().c_str(),
                            config_.getMySQLPassword().c_str(),
                            config_.getMySQLDatabase().c_str(),
                            config_.getMySQLPort(),
                            nullptr, 0)) {
        spdlog::error("MySQL connection error: {}", mysql_error(connection_));
        mysql_close(connection_);
        connection_ = nullptr;
        return false;
                            }

    spdlog::info("Successfully connected to MySQL database.");
    return true;
}

bool DatabaseManager::isConnected() const {
    return connection_ != nullptr && mysql_ping(connection_) == 0;
}

void DatabaseManager::reconnect() {
    if (!isConnected()) {
        spdlog::info("Reconnecting to MySQL...");
        connect();
    }
}

MYSQL* DatabaseManager::getConnection() {
    return connection_;
}
