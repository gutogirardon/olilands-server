#ifndef WORLD_H
#define WORLD_H

#include "mapmanager.h"
#include <vector>
#include <tuple>

class World {
public:
    World();
    ~World();

    bool initialize(const std::string& mapFilePath);
    void updatePlayerPosition(int playerId, int x, int y);
    std::tuple<int, int, int> getPlayerPosition(int playerId) const;
    std::vector<int> getPlayersInProximity(int playerId, int range) const;
    bool isPositionWalkable(int x, int y) const;

private:
    MapManager mapManager;
};

#endif // WORLD_H
