#ifndef CONNECTIONGUARD_H
#define CONNECTIONGUARD_H

#include "databasemanager.h"
#include <chrono>
#include <stdexcept>

class ConnectionGuard {
public:
    ConnectionGuard(DatabaseManager& dbManager, std::chrono::milliseconds timeout = std::chrono::milliseconds(1000))
        : dbManager_(dbManager), conn_(nullptr) {
        conn_ = dbManager_.getConnection(timeout);
        if (!conn_) {
            throw std::runtime_error("Falha ao adquirir conexão do pool: timeout");
        }
    }

    ~ConnectionGuard() {
        if (conn_) {
            dbManager_.releaseConnection(conn_);
        }
    }

    MYSQL* get() const {
        return conn_;
    }

    // Impede a cópia
    ConnectionGuard(const ConnectionGuard&) = delete;
    ConnectionGuard& operator=(const ConnectionGuard&) = delete;

    // Permite a movimentação
    ConnectionGuard(ConnectionGuard&& other) noexcept
        : dbManager_(other.dbManager_), conn_(other.conn_) {
        other.conn_ = nullptr;
    }

    ConnectionGuard& operator=(ConnectionGuard&& other) noexcept {
        if (this != &other) {
            if (conn_) {
                dbManager_.releaseConnection(conn_);
            }
            conn_ = other.conn_;
            other.conn_ = nullptr;
        }
        return *this;
    }

private:
    DatabaseManager& dbManager_;
    MYSQL* conn_;
};

#endif // CONNECTIONGUARD_H
