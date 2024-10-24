#include "characterprotocol.h"
#include <spdlog/spdlog.h>

ProtocolCommand CharacterProtocol::getCommandFromMessage(const std::vector<uint8_t>& message) {
    return extractCommandFromMessage(message);
}

void CharacterProtocol::handleCharacterListRequest(const std::vector<uint8_t>& message) {
    spdlog::info("Received character list request.");
    // A implementação real será feita na sessão, este método é apenas ilustrativo
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

void CharacterProtocol::handleProtocolCommand(const std::vector<uint8_t>& message) {
    ProtocolCommand command = extractCommandFromMessage(message);
    switch (command) {
        case ProtocolCommand::REQUEST_CHARACTER_LIST:
            handleCharacterListRequest(message);
            break;
        case ProtocolCommand::SELECT_CHARACTER:
            handleCharacterSelectionRequest(message);
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
