#ifndef USERACCOUNTMANAGER_H
#define USERACCOUNTMANAGER_H

#include "databasemanager.h"
#include <string>

class UserAccountManager {
public:
    explicit UserAccountManager(DatabaseManager& dbManager);

    // Método para validar o login do usuário
    bool validateLogin(const std::string& username, const std::string& password, int& account_id);

private:
    DatabaseManager& dbManager_;

    // Método auxiliar para gerar o hash da senha
    static std::string hashPassword(const std::string& password);
};

#endif // USERACCOUNTMANAGER_H
