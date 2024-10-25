#include "maploader.h"
#include <spdlog/spdlog.h>
#include <chrono>

MapLoader::MapLoader()
    : map_(std::make_unique<tmx::Map>())
{
}

MapLoader::~MapLoader() = default;

bool MapLoader::loadMap(const std::string& filePath) {
    auto startTime = std::chrono::high_resolution_clock::now();

    if (!map_->load(filePath)) {
        spdlog::error("Failed to load map from file: {}", filePath);
        return false;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

    spdlog::info("Map loaded successfully from file: {} (in {} ms)", filePath, duration);

    // Obter informações do mapa
    const auto& mapBounds = map_->getBounds();
    const auto& mapProperties = map_->getProperties();
    const auto& mapSize = map_->getTileCount();
    const auto& tileSize = map_->getTileSize();
    const auto& layers = map_->getLayers();

    spdlog::info("Map Dimensions: {} x {} tiles", mapSize.x, mapSize.y);
    spdlog::info("Tile Size: {} x {} pixels", tileSize.x, tileSize.y);
    spdlog::info("Number of Layers: {}", layers.size());

    // Obter tilesets
    const auto& tilesets = map_->getTilesets();
    spdlog::info("Number of Tilesets: {}", tilesets.size());
    for (const auto& tileset : tilesets) {
        spdlog::info("Tileset: {} (First GID: {})", tileset.getName(), tileset.getFirstGID());
    }

    // Obter propriedades do mapa
    if (!mapProperties.empty()) {
        spdlog::info("Map Properties:");
        for (const auto& property : mapProperties) {
            std::string value;
            switch (property.getType()) {
                case tmx::Property::Type::String:
                    value = property.getStringValue();
                    break;
                case tmx::Property::Type::Int:
                    value = std::to_string(property.getIntValue());
                    break;
                case tmx::Property::Type::Float:
                    value = std::to_string(property.getFloatValue());
                    break;
                case tmx::Property::Type::Boolean:
                    value = property.getBoolValue() ? "true" : "false";
                    break;
                case tmx::Property::Type::File:
                    value = property.getFileValue();
                    break;
                default:
                    value = "Unknown Type";
                    break;
            }
            spdlog::info(" - {} : {}", property.getName(), value);
        }
    }

    // Processar as camadas do mapa
    for (const auto& layer : layers) {
        spdlog::info("Layer: {}", layer->getName());
        spdlog::info(" - Type: {}", static_cast<int>(layer->getType()));
        spdlog::info(" - Opacity: {}", layer->getOpacity());
        spdlog::info(" - Visible: {}", layer->getVisible());

        // Obter propriedades da camada
        const auto& layerProperties = layer->getProperties();
        if (!layerProperties.empty()) {
            spdlog::info(" - Layer Properties:");
            for (const auto& property : layerProperties) {
                std::string value;
                switch (property.getType()) {
                    case tmx::Property::Type::String:
                        value = property.getStringValue();
                        break;
                    case tmx::Property::Type::Int:
                        value = std::to_string(property.getIntValue());
                        break;
                    case tmx::Property::Type::Float:
                        value = std::to_string(property.getFloatValue());
                        break;
                    case tmx::Property::Type::Boolean:
                        value = property.getBoolValue() ? "true" : "false";
                        break;
                    case tmx::Property::Type::File:
                        value = property.getFileValue();
                        break;
                    default:
                        value = "Unknown Type";
                        break;
                }
                spdlog::info("   - {} : {}", property.getName(), value);
            }
        }

        // Processar camada conforme o tipo
        if (layer->getType() == tmx::Layer::Type::Tile) {
            spdlog::info("Processing tile layer: {}", layer->getName());
            // Processar os tiles da camada, se necessário
        }
        else if (layer->getType() == tmx::Layer::Type::Object) {
            spdlog::info("Processing object layer: {}", layer->getName());
            // Processar os objetos da camada, se necessário
        }
        else if (layer->getType() == tmx::Layer::Type::Image) {
            spdlog::info("Processing image layer: {}", layer->getName());
            // Processar a camada de imagem, se necessário
        }
        else if (layer->getType() == tmx::Layer::Type::Group) {
            spdlog::info("Processing group layer: {}", layer->getName());
            // Processar as camadas do grupo, se necessário
        }
    }

    return true;
}

const tmx::Map& MapLoader::getMap() const {
    return *map_;
}