#ifndef MAPLOADER_H
#define MAPLOADER_H

#include <tmxlite/Map.hpp>

class MapLoader {
public:
    MapLoader();
    ~MapLoader();

    bool loadMap(const std::string& filePath);

    const tmx::Map& getMap() const;

private:
    std::unique_ptr<tmx::Map> map_;
};

#endif // MAPLOADER_H
