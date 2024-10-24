#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "connectionpool.h"
#include "config/configmanager.h"

class DatabaseManager {
public:
    DatabaseManager(const ConfigManager& config);
    ~DatabaseManager();

    bool connect();
    bool isConnected() const;
    MYSQL* getConnection();
    void releaseConnection(MYSQL* conn);

private:
    const ConfigManager& config_;
    std::unique_ptr<ConnectionPool> connectionPool_;  // Usar pool de conexões
};

#endif // DATABASEMANAGER_H
