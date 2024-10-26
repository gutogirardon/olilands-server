#include "mapmanager.h"
#include <spdlog/spdlog.h>
#include <cmath>

MapManager::MapManager() = default;

MapManager::~MapManager() = default;

bool MapManager::initialize(const std::string& mapFilePath) {
    if (!mapLoader.loadMap(mapFilePath)) {
        spdlog::error("MapManager failed to load map.");
        return false;
    }

    const tmx::Map& map = mapLoader.getMap();

    // Obter dimensões do mapa
    mapWidth = map.getTileCount().x;
    mapHeight = map.getTileCount().y;
    tileWidth = map.getTileSize().x;
    tileHeight = map.getTileSize().y;

    // Inicializar o grid de colisão como caminhável
    collisionGrid.resize(mapHeight, std::vector<bool>(mapWidth, true));

    // Processar as áreas de colisão
    const auto& collisionAreas = mapLoader.getCollisionAreas();
    for (const auto& rect : collisionAreas) {
        int startX = static_cast<int>(rect.left / tileWidth);
        int startY = static_cast<int>(rect.top / tileHeight);
        int endX = static_cast<int>((rect.left + rect.width) / tileWidth);
        int endY = static_cast<int>((rect.top + rect.height) / tileHeight);

        spdlog::debug("Processing collision area from ({}, {}) to ({}, {})", startX, startY, endX, endY);


        for (int y = startY; y <= endY; ++y) {
            for (int x = startX; x <= endX; ++x) {
                if (x >= 0 && x < mapWidth && y >= 0 && y < mapHeight) {
                    collisionGrid[y][x] = false; // Marca como não caminhável
                }
            }
        }
    }

    // Processar camadas de tiles, se necessário
    const auto& layers = map.getLayers();
    for (const auto& layer : layers) {
        if (layer->getType() == tmx::Layer::Type::Tile) {
            const auto& tileLayer = layer->getLayerAs<tmx::TileLayer>();
            TileLayer newLayer(tileLayer);
            tileLayers.push_back(std::move(newLayer));
        }
    }
    return true;
}

bool MapManager::isPositionWalkable(int x, int y) const {
    if (x >= 0 && x < mapWidth && y >= 0 && y < mapHeight) {
        return collisionGrid[y][x];
    }
    return false; // Posições fora do mapa não são caminháveis
}

const std::vector<TileLayer>& MapManager::getTileLayers() const {
    return tileLayers;
}

void MapManager::updatePlayerPosition(int playerId, int x, int y) {
    playerPositions[playerId] = {x, y};
    spdlog::info("Updated position for playerId {}: ({}, {})", playerId, x, y);
}

std::tuple<int, int, int> MapManager::getPlayerPosition(int playerId) const {
    auto it = playerPositions.find(playerId);
    if (it != playerPositions.end()) {
        return {it->second.first, it->second.second, 0};
    } else {
        spdlog::warn("Player ID {} not found in playerPositions", playerId);
        return {0, 0, 0};
    }
}

std::vector<int> MapManager::getPlayersInProximity(int playerId, int range) const {
    std::vector<int> nearbyPlayers;
    auto it = playerPositions.find(playerId);
    if (it == playerPositions.end()) {
        spdlog::warn("Player ID {} not found in positions", playerId);
        return nearbyPlayers;
    }

    int x = it->second.first;
    int y = it->second.second;

    for (const auto& [otherPlayerId, position] : playerPositions) {
        if (otherPlayerId == playerId) continue;

        int dx = position.first - x;
        int dy = position.second - y;
        int distance = static_cast<int>(std::sqrt(dx * dx + dy * dy));

        if (distance <= range) {
            nearbyPlayers.push_back(otherPlayerId);
        }
    }

    spdlog::info("Found {} players near playerId {} within range {}", nearbyPlayers.size(), playerId, range);
    return nearbyPlayers;
}
