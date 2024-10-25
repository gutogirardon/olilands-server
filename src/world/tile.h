#ifndef TILE_H
#define TILE_H

#include <tmxlite/TileLayer.hpp>
#include <cstdint>

class Tile {
public:
    Tile(const tmx::TileLayer::Tile& tileData);
    ~Tile();

    uint32_t getID() const;
    bool isFlippedHorizontally() const;
    bool isFlippedVertically() const;
    bool isFlippedDiagonally() const;

private:
    uint32_t ID;
    bool flippedHorizontally;
    bool flippedVertically;
    bool flippedDiagonally;
};

#endif // TILE_H
