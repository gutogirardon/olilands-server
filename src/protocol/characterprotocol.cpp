#include "characterprotocol.h"
#include "movementprotocol.h"
#include <spdlog/spdlog.h>
#include <cstring>

ProtocolCommand CharacterProtocol::getCommandFromMessage(const std::vector<uint8_t>& message) {
    return extractCommandFromMessage(message);
}

void CharacterProtocol::handleCharacterListRequest(const std::vector<uint8_t>& message) {
    spdlog::info("Received character list request.");
    // A implementação real será feita na sessão, este método é apenas ilustrativo
}

void CharacterProtocol::handleMovementRequest(const std::vector<uint8_t>& message) {
    try {
        MovementProtocol movementProtocol;
        auto [x, y] = movementProtocol.extractMovementData(message);
        spdlog::info("Character movement request received: x = {}, y = {}", x, y);

        // Aqui você pode adicionar o processamento para atualizar a posição no servidor,
        // como atualizar a posição do jogador e informar aos jogadores próximos
    } catch (const std::exception& e) {
        spdlog::error("Error processing movement request: {}", e.what());
    }
}

CharacterInfo CharacterProtocol::handleCharacterSelectionRequest(const std::vector<uint8_t>& message) {
    CharacterInfo characterInfo;

    try {
        ProtocolCommand command = extractCommandFromMessage(message);

        if (command == ProtocolCommand::SELECT_CHARACTER) {
            characterInfo.name = extractDataString(message, 1);
            spdlog::info("Character selection attempt: Name: {}", characterInfo.name);
        } else {
            spdlog::error("Unexpected command received: {}", static_cast<int>(command));
        }
    } catch (const std::exception& e) {
        spdlog::error("Error processing character selection request: {}", e.what());
    }

    return characterInfo;
}

CharacterCreationInfo CharacterProtocol::handleCharacterCreationRequest(const std::vector<uint8_t>& message) {
    CharacterCreationInfo creationInfo;

    try {
        ProtocolCommand command = extractCommandFromMessage(message);

        if (command == ProtocolCommand::CREATE_CHARACTER) {
            size_t offset = 1;
            creationInfo.name = extractDataString(message, offset);
            offset += creationInfo.name.size() + 1;
            creationInfo.vocation = extractDataString(message, offset);

            spdlog::info("Character creation attempt: Name: {}, Vocation: {}", creationInfo.name, creationInfo.vocation);
        } else {
            spdlog::error("Unexpected command received: {}", static_cast<int>(command));
        }
    } catch (const std::exception& e) {
        spdlog::error("Error processing character creation request: {}", e.what());
    }

    return creationInfo;
}

void CharacterProtocol::handleProtocolCommand(const std::vector<uint8_t>& message) {
    ProtocolCommand command = extractCommandFromMessage(message);
    switch (command) {
    case ProtocolCommand::REQUEST_CHARACTER_LIST:
        handleCharacterListRequest(message);
        break;
    case ProtocolCommand::SELECT_CHARACTER:
        handleCharacterSelectionRequest(message);
        break;
    case ProtocolCommand::CREATE_CHARACTER:
        handleCharacterCreationRequest(message);
        break;
    case ProtocolCommand::MOVE_CHARACTER: //@todo validar se isso está sendo usado
        handleMovementRequest(message);
        break;
    default:
        spdlog::error("Unhandled character protocol command: {}", static_cast<int>(command));
        break;
    }
}

std::string CharacterProtocol::extractDataString(const std::vector<uint8_t>& message, size_t start) {
    size_t length = 0;
    while (start + length < message.size() && message[start + length] != 0) {
        length++;
    }
    return {reinterpret_cast<const char*>(&message[start]), length};
}

int CharacterProtocol::extractDataInt(const std::vector<uint8_t>& message, size_t start) {
    if (start + sizeof(int) > message.size()) {
        throw std::runtime_error("Not enough data to extract int");
    }
    int value;
    std::memcpy(&value, &message[start], sizeof(int));
    return value;
}

std::vector<uint8_t> CharacterProtocol::createCharacterList(const std::vector<CharacterInfo>& characters) {
    CharacterListResponse response;
    response.numCharacters = static_cast<uint16_t>(characters.size());
    response.characters = characters;

    std::vector<uint8_t> data;
    data.push_back(static_cast<uint8_t>(response.command));

    // Serialize numCharacters (2 bytes, big-endian)
    data.push_back((response.numCharacters >> 8) & 0xFF);
    data.push_back(response.numCharacters & 0xFF);

    for (const auto& character : characters) {
        // Serialize characterId (4 bytes, big-endian)
        data.push_back((character.id >> 24) & 0xFF);
        data.push_back((character.id >> 16) & 0xFF);
        data.push_back((character.id >> 8) & 0xFF);
        data.push_back(character.id & 0xFF);

        // Serialize name (null-terminated string)
        data.insert(data.end(), character.name.begin(), character.name.end());
        data.push_back(0); // Null terminator

        // Serialize vocation (null-terminated string)
        data.insert(data.end(), character.vocation.begin(), character.vocation.end());
        data.push_back(0); // Null terminator

        // Serialize level (4 bytes, big-endian)
        data.push_back((character.level >> 24) & 0xFF);
        data.push_back((character.level >> 16) & 0xFF);
        data.push_back((character.level >> 8) & 0xFF);
        data.push_back(character.level & 0xFF);
    }

    spdlog::info("Created CharacterListResponse with {} characters.", response.numCharacters);
    return data;
}

std::vector<uint8_t> CharacterProtocol::createCharacterSelectionSuccess(uint32_t characterId) {
    CharacterSelectionSuccessResponse response;
    response.characterId = characterId;

    std::vector<uint8_t> data;
    data.push_back(static_cast<uint8_t>(response.command));

    // Serialize characterId (4 bytes, big-endian)
    data.push_back((response.characterId >> 24) & 0xFF);
    data.push_back((response.characterId >> 16) & 0xFF);
    data.push_back((response.characterId >> 8) & 0xFF);
    data.push_back(response.characterId & 0xFF);

    spdlog::info("Created CharacterSelectionSuccessResponse: characterId = {}", response.characterId);
    return data;
}

std::vector<uint8_t> CharacterProtocol::createCharacterSelectionFailure(uint8_t errorCode) {
    CharacterSelectionFailureResponse response;
    response.errorCode = errorCode;

    std::vector<uint8_t> data;
    data.push_back(static_cast<uint8_t>(response.command));
    data.push_back(response.errorCode);

    spdlog::info("Created CharacterSelectionFailureResponse: errorCode = {}", static_cast<int>(response.errorCode));
    return data;
}

std::vector<uint8_t> CharacterProtocol::createCharacterCreationSuccess(uint32_t characterId) {
    CharacterCreationSuccessResponse response;
    response.characterId = characterId;

    std::vector<uint8_t> data;
    data.push_back(static_cast<uint8_t>(response.command));

    // Serialize characterId (4 bytes, big-endian)
    data.push_back((response.characterId >> 24) & 0xFF);
    data.push_back((response.characterId >> 16) & 0xFF);
    data.push_back((response.characterId >> 8) & 0xFF);
    data.push_back(response.characterId & 0xFF);

    spdlog::info("Created CharacterCreationSuccessResponse: characterId = {}", response.characterId);
    return data;
}

std::vector<uint8_t> CharacterProtocol::createCharacterCreationFailure(uint8_t errorCode) {
    CharacterCreationFailureResponse response;
    response.errorCode = errorCode;

    std::vector<uint8_t> data;
    data.push_back(static_cast<uint8_t>(response.command));
    data.push_back(response.errorCode);

    spdlog::info("Created CharacterCreationFailureResponse: errorCode = {}", static_cast<int>(response.errorCode));
    return data;
}
