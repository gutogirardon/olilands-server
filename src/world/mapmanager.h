#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include "tilelayer.h"
#include "maploader.h"
#include <vector>
#include <unordered_map>

class MapManager {
public:
    MapManager();
    ~MapManager();

    bool initialize(const std::string& mapFilePath);
    const std::vector<TileLayer>& getTileLayers() const;

    void updatePlayerPosition(int playerId, int x, int y);
    std::vector<int> getPlayersInProximity(int playerId, int range) const;

private:
    std::vector<TileLayer> tileLayers;
    std::unordered_map<int, std::pair<int, int>> playerPositions;
    MapLoader mapLoader;
};

#endif // MAPMANAGER_H
