#include "tilelayer.h"

TileLayer::TileLayer(const tmx::TileLayer& layer)
    : name(layer.getName())
{
    const auto& tmxTiles = layer.getTiles();
    tiles.reserve(tmxTiles.size());

    for (const auto& tmxTile : tmxTiles) {
        Tile tile(tmxTile);
        tiles.push_back(std::move(tile));
    }
}

TileLayer::~TileLayer() = default;

const std::vector<Tile>& TileLayer::getTiles() const {
    return tiles;
}

const std::string& TileLayer::getName() const {
    return name;
}
