#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include "maploader.h"
#include "tilelayer.h"
#include <vector>
#include <string>

class MapManager {
public:
    MapManager();
    ~MapManager();

    bool initialize(const std::string& mapFilePath);

    const std::vector<TileLayer>& getTileLayers() const;

private:
    MapLoader mapLoader;
    std::vector<TileLayer> tileLayers;
};

#endif // MAPMANAGER_H
