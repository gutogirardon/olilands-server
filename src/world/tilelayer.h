#ifndef TILELAYER_H
#define TILELAYER_H

#include "tile.h"
#include <vector>
#include <string>
#include <tmxlite/TileLayer.hpp>

class TileLayer {
public:
    TileLayer(const tmx::TileLayer& layer);
    ~TileLayer();

    const std::vector<Tile>& getTiles() const;
    const std::string& getName() const;

private:
    std::string name;
    std::vector<Tile> tiles;
};

#endif // TILELAYER_H
