#ifndef PLAYERMANAGER_H
#define PLAYERMANAGER_H

#include "databasemanager.h"
#include <string>

class PlayerManager {
public:
    PlayerManager(DatabaseManager& dbManager);

    bool createCharacter(const std::string& name, const std::string& vocation, int account_id);

private:
    DatabaseManager& dbManager_;
};

#endif // PLAYERMANAGER_H
