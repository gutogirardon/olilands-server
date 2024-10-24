#ifndef PLAYERMANAGER_H
#define PLAYERMANAGER_H

#include "databasemanager.h"
#include "models/characterinfo.h"
#include <vector>

class PlayerManager {
public:
    PlayerManager(DatabaseManager& dbManager);

    bool createCharacter(const std::string& name, const std::string& vocation, int account_id);

    // Novo método para obter a lista de personagens
    std::vector<CharacterInfo> getCharactersForAccount(int account_id);

    // Método para carregar dados de um personagem específico
    CharacterInfo getCharacterInfo(const std::string& characterName, int account_id);

private:
    DatabaseManager& dbManager_;
};

#endif // PLAYERMANAGER_H
