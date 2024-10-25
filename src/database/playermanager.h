#ifndef PLAYERMANAGER_H
#define PLAYERMANAGER_H

#include "databasemanager.h"
#include "models/characterinfo.h"
#include <vector>

class PlayerManager {
public:
    PlayerManager(DatabaseManager& dbManager);

    bool createCharacter(const std::string& name, const std::string& vocation, int account_id);

    std::vector<CharacterInfo> getCharactersForAccount(int account_id);
    CharacterInfo getCharacterInfo(const std::string& characterName, int account_id);

    // Novo método para recuperar posição do jogador
    std::tuple<int, int, int> getPlayerPosition(int player_id);

    // Novo método para atualizar posição do jogador
    bool updatePlayerPosition(int player_id, int pos_x, int pos_y, int pos_z);

private:
    DatabaseManager& dbManager_;
};

#endif // PLAYERMANAGER_H
