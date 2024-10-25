#include "mapmanager.h"
#include <spdlog/spdlog.h>

MapManager::MapManager() = default;

MapManager::~MapManager() = default;

bool MapManager::initialize(const std::string& mapFilePath) {
    if (!mapLoader.loadMap(mapFilePath)) {
        spdlog::error("MapManager failed to load map.");
        return false;
    }

    const tmx::Map& map = mapLoader.getMap();

    // Processar as camadas de tiles
    const auto& layers = map.getLayers();
    for (const auto& layer : layers) {
        if (layer->getType() == tmx::Layer::Type::Tile) {
            const auto& tileLayer = layer->getLayerAs<tmx::TileLayer>();
            TileLayer newLayer(tileLayer);
            tileLayers.push_back(std::move(newLayer));
        }
        // Processar outros tipos de camada, se necessário
    }

    return true;
}

const std::vector<TileLayer>& MapManager::getTileLayers() const {
    return tileLayers;
}
