#include "loginprotocol.h"

PlayerLoginInfo LoginProtocol::handleLoginRequest(const std::vector<uint8_t>& message) {
    PlayerLoginInfo loginInfo;

    try {
        ProtocolCommand command = extractCommandFromMessage(message); // Usa o novo nome do método

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

void LoginProtocol::handleProtocolCommand(const std::vector<uint8_t>& message) {
    handleLoginRequest(message);  // Aqui, pode-se expandir para outros tipos de comandos no futuro
}

std::string LoginProtocol::extractDataString(const std::vector<uint8_t>& message, size_t start) {
    size_t length = 0;
    while (start + length < message.size() && message[start + length] != 0) {
        length++;
    }
    return {reinterpret_cast<const char*>(&message[start]), length};
}
