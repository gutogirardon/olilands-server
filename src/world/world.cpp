#include "world.h"

World::World() = default;

World::~World() = default;

bool World::initialize(const std::string& mapFilePath) {
    if (!mapManager.initialize(mapFilePath)) {
        return false;
    }

    // Inicializar outros componentes do mundo, se necessário

    return true;
}
