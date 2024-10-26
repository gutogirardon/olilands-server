// maploader.h
#pragma once

#include <tmxlite/Map.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/ObjectGroup.hpp>
#include <vector>

class MapLoader {
public:
    MapLoader();
    ~MapLoader();

    bool loadMap(const std::string& filePath);
    const tmx::Map& getMap() const;

    // Novo método para obter as áreas de colisão
    const std::vector<tmx::FloatRect>& getCollisionAreas() const;

private:
    std::unique_ptr<tmx::Map> map_;
    std::vector<tmx::FloatRect> collisionAreas_; // Armazenar áreas de colisão
};
