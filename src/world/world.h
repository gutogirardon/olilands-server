// world.h
#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <tuple>
#include <unordered_map>
#include <mutex>
#include "mapmanager.h"

enum class PlayerState : uint8_t {
    Idle = 0,
    Moving = 1,
    Attacking = 2,
};

class World {
public:
    World();
    ~World();

    bool initialize(const std::string& mapFilePath);
    void updatePlayerPosition(int playerId, int x, int y);
    std::tuple<int, int, int> getPlayerPosition(int playerId) const;
    std::vector<int> getPlayersInProximity(int playerId, int range) const;
    bool isPositionWalkable(int x, int y) const;

    // **Métodos para Gerenciar Estados dos Jogadores**
    void setPlayerState(int playerId, PlayerState state);
    PlayerState getPlayerState(int playerId) const;

private:
    MapManager mapManager;

    mutable std::unordered_map<int, PlayerState> playerStates_;
    mutable std::mutex stateMutex_;
};

#endif // WORLD_H
