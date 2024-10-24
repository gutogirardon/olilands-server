#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <mysql/mysql.h>
#include "config/configmanager.h"

class DatabaseManager {
public:
    DatabaseManager(const ConfigManager& config);
    ~DatabaseManager();

    bool connect();
    bool isConnected() const;
    void reconnect();
    MYSQL* getConnection();

private:
    const ConfigManager& config_;
    MYSQL* connection_;
};

#endif // DATABASEMANAGER_H
