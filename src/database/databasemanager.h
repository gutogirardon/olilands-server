#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "connectionpool.h"
#include "config/configmanager.h"
#include <memory>

class DatabaseManager {
public:
    DatabaseManager(const ConfigManager& config);
    ~DatabaseManager();

    bool connect();
    bool isConnected() const;
    MYSQL* getConnection(std::chrono::milliseconds timeout = std::chrono::milliseconds(1000));
    void releaseConnection(MYSQL* conn);

private:
    const ConfigManager& config_;
    std::unique_ptr<ConnectionPool> connectionPool_;
};

#endif // DATABASEMANAGER_H
