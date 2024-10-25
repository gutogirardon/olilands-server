#include "tile.h"

Tile::Tile(const tmx::TileLayer::Tile& tileData)
    : ID(tileData.ID)
    , flippedHorizontally(tileData.flipFlags & tmx::TileLayer::FlipFlag::Horizontal)
    , flippedVertically(tileData.flipFlags & tmx::TileLayer::FlipFlag::Vertical)
    , flippedDiagonally(tileData.flipFlags & tmx::TileLayer::FlipFlag::Diagonal)
{
}

Tile::~Tile() = default;

uint32_t Tile::getID() const {
    return ID;
}

bool Tile::isFlippedHorizontally() const {
    return flippedHorizontally;
}

bool Tile::isFlippedVertically() const {
    return flippedVertically;
}

bool Tile::isFlippedDiagonally() const {
    return flippedDiagonally;
}
