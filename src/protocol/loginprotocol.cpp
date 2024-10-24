#include "loginprotocol.h"
#include "protocolenum.h"

ProtocolCommand LoginProtocol::getCommandFromMessage(const std::vector<uint8_t>& message) {
    return extractCommandFromMessage(message);  // Chama o método protegido da classe base
}

PlayerLoginInfo LoginProtocol::handleLoginRequest(const std::vector<uint8_t>& message) {
    PlayerLoginInfo loginInfo;

    try {
        ProtocolCommand command = extractCommandFromMessage(message);

        if (command == ProtocolCommand::LOGIN) {
            loginInfo.username = extractDataString(message, 1);
            loginInfo.password = extractDataString(message, 1 + loginInfo.username.size() + 1);

            spdlog::info("Login attempt with username: {} and password: {}", loginInfo.username, loginInfo.password);
        } else {
            spdlog::error("Unexpected command received: {}", static_cast<int>(command));
        }
    } catch (const std::exception& e) {
        spdlog::error("Error processing login request: {}", e.what());
    }

    return loginInfo;
}

PlayerCreationInfo LoginProtocol::handleCharacterCreationRequest(const std::vector<uint8_t>& message) {
    PlayerCreationInfo creationInfo;

    try {
        ProtocolCommand command = extractCommandFromMessage(message);

        if (command == ProtocolCommand::CREATE_CHARACTER) {
            creationInfo.name = extractDataString(message, 1);
            creationInfo.vocation = extractDataString(message, 1 + creationInfo.name.size() + 1);

            spdlog::info("Character creation attempt: Name: {}, Vocation: {}", creationInfo.name, creationInfo.vocation);
        } else {
            spdlog::error("Unexpected command received: {}", static_cast<int>(command));
        }
    } catch (const std::exception& e) {
        spdlog::error("Error processing character creation request: {}", e.what());
    }

    return creationInfo;
}

void LoginProtocol::handleProtocolCommand(const std::vector<uint8_t>& message) {
    switch (ProtocolCommand command = extractCommandFromMessage(message)) {
        case ProtocolCommand::LOGIN:
            handleLoginRequest(message);
            break;
        case ProtocolCommand::CREATE_CHARACTER:
            handleCharacterCreationRequest(message);
            break;
        default:
            spdlog::error("Unhandled protocol command: {}", static_cast<int>(command));
            break;
    }
}

std::string LoginProtocol::extractDataString(const std::vector<uint8_t>& message, size_t start) {
    size_t length = 0;
    while (start + length < message.size() && message[start + length] != 0) {
        length++;
    }
    return {reinterpret_cast<const char*>(&message[start]), length};
}
