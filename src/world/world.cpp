// world.cpp
#include "world.h"
#include <spdlog/spdlog.h>
#include <cmath>
#include <algorithm>

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

std::tuple<int, int, int> World::getPlayerPosition(int playerId) const {
    return mapManager.getPlayerPosition(playerId);
}

std::vector<int> World::getPlayersInProximity(int playerId, int range) const {
    return mapManager.getPlayersInProximity(playerId, range);
}

bool World::isPositionWalkable(int x, int y) const {
    return mapManager.isPositionWalkable(x, y);
}

// **Implementação dos Métodos de Estado**
void World::setPlayerState(int playerId, PlayerState state) {
    std::lock_guard<std::mutex> lock(stateMutex_);
    playerStates_[playerId] = state;
    spdlog::info("Player {} state set to {}", playerId, static_cast<int>(state));
}

PlayerState World::getPlayerState(int playerId) const {
    std::lock_guard<std::mutex> lock(stateMutex_);
    auto it = playerStates_.find(playerId);
    if (it != playerStates_.end()) {
        return it->second;
    } else {
        spdlog::warn("Player {} state not found. Defaulting to Idle.", playerId);
        return PlayerState::Idle;
    }
}
