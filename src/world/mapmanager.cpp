// mapmanager.cpp
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

    // Obter dimensões do mapa em pixels
    mapWidth = static_cast<int>(map.getBounds().width);
    mapHeight = static_cast<int>(map.getBounds().height);

    // Obter tamanho do tile
    tileWidth = map.getTileSize().x;
    tileHeight = map.getTileSize().y;

    spdlog::info("Map Info: width: {} pixels, height {} pixels", mapWidth, mapHeight);
    spdlog::info("Tile Info: width: {}, height {}", tileWidth, tileHeight);

    // Inicializar o grid de colisão diretamente em pixels
    collisionGrid.resize(mapHeight, std::vector<bool>(mapWidth, true));

    // Processar as áreas de colisão (agora em pixels)
    const auto& collisionAreas = mapLoader.getCollisionAreas();
    for (const auto& rect : collisionAreas) {
        int startX = static_cast<int>(rect.left);
        int startY = static_cast<int>(rect.top);
        int endX = static_cast<int>(rect.left + rect.width);
        int endY = static_cast<int>(rect.top + rect.height);

        spdlog::debug("Processing collision area from ({}, {}) to ({}, {})", startX, startY, endX, endY);

        for (int y = startY; y <= endY; ++y) {
            for (int x = startX; x <= endX; ++x) {
                if (x >= 0 && x < mapWidth && y >= 0 && y < mapHeight) {
                    collisionGrid[y][x] = false; // Marca o pixel como não caminhável
                }
            }
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

    int rangeSquared = range * range; // Calcular o quadrado do range uma vez

    for (const auto& [otherPlayerId, position] : playerPositions) {
        if (otherPlayerId == playerId) continue;

        int dx = position.first - x;
        int dy = position.second - y;
        int distanceSquared = dx * dx + dy * dy;

        if (distanceSquared <= rangeSquared) { // Comparar com o quadrado do range
            nearbyPlayers.push_back(otherPlayerId);
        }
    }

    spdlog::info("Found {} players near playerId {} within range {}", nearbyPlayers.size(), playerId, range);
    return nearbyPlayers;
}