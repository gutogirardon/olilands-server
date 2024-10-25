#include "world.h"

World::World() = default;

World::~World() = default;

bool World::initialize(const std::string& mapFilePath) {
    if (!mapManager.initialize(mapFilePath)) {
        return false;
    }

    return true;
}

void World::updatePlayerPosition(int playerId, int x, int y) {
    mapManager.updatePlayerPosition(playerId, x, y);
}

std::vector<int> World::getPlayersInProximity(int playerId, int range) const {
    return mapManager.getPlayersInProximity(playerId, range);
}