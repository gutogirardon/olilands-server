#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <string>
#include "databasemanager.h"

class UserManager {
public:
    UserManager(DatabaseManager& dbManager);

    bool saveUserData(int userId, const std::string& data);
    std::string getUserData(int userId);

private:
    DatabaseManager& dbManager_;
};

#endif // USERMANAGER_H
