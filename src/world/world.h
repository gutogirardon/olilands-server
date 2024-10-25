#ifndef WORLD_H
#define WORLD_H

#include "mapmanager.h"
#include <vector>

class World {
public:
    World();
    ~World();

    bool initialize(const std::string& mapFilePath);

    // Atualiza a posição do jogador no mapa
    void updatePlayerPosition(int playerId, int x, int y);

    std::vector<int> getPlayersInProximity(int playerId, int range) const;


private:
    MapManager mapManager;
};

#endif // WORLD_H
