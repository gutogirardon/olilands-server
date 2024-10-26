#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include "tilelayer.h"
#include "maploader.h"
#include <vector>
#include <unordered_map>
#include <tmxlite/Map.hpp>

class MapManager {
public:
    MapManager();
    ~MapManager();

    bool initialize(const std::string& mapFilePath);
    const std::vector<TileLayer>& getTileLayers() const;

    void updatePlayerPosition(int playerId, int x, int y);
    std::tuple<int, int, int> getPlayerPosition(int playerId) const;
    std::vector<int> getPlayersInProximity(int playerId, int range) const;

    // Novo método para verificar se a posição é caminhável
    bool isPositionWalkable(int x, int y) const;

private:
    MapLoader mapLoader;
    std::vector<TileLayer> tileLayers;
    std::unordered_map<int, std::pair<int, int>> playerPositions;

    // Grid de colisão
    std::vector<std::vector<bool>> collisionGrid;
    int mapWidth;
    int mapHeight;
    int tileWidth;
    int tileHeight;
};

#endif // MAPMANAGER_H
