#ifndef WORLD_H
#define WORLD_H

#include "mapmanager.h"
#include <string>

class World {
public:
    World();
    ~World();

    bool initialize(const std::string& mapFilePath);

private:
    MapManager mapManager;
};

#endif // WORLD_H
