// playermanager.h
#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H

#include "models/characterinfo.h"
#include "databasemanager.h"
#include <vector>
#include <string>
#include <cstdint>

class PlayerManager {
public:
    PlayerManager(DatabaseManager& dbManager);
    ~PlayerManager();

    // Métodos existentes
    bool createCharacter(const std::string& name, const std::string& vocation, int account_id);
    std::vector<CharacterInfo> getCharactersForAccount(int account_id);
    CharacterInfo getCharacterInfo(const std::string& characterName, int account_id);
    std::tuple<int, int, int> getPlayerPosition(int player_id);
    bool updatePlayerPosition(int player_id, int pos_x, int pos_y, int pos_z);
    CharacterInfo getCharacterInfoById(int player_id);

private:
    DatabaseManager& dbManager_;

};

#endif // PLAYER_MANAGER_H
