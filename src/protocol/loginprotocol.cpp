#include "loginprotocol.h"

LoginCredentials LoginProtocol::processLoginMessage(const std::vector<uint8_t>& message) {
    LoginCredentials credentials;

    try {
        ProtocolCommand command = readCommand(message); // Chama o método protegido da classe base

        if (command == ProtocolCommand::LOGIN) {
            credentials.username = extractString(message, 1);
            credentials.password = extractString(message, 1 + credentials.username.size() + 1);

            spdlog::info("Login attempt with username: {} and password: {}", credentials.username, credentials.password);
        } else {
            spdlog::error("Unexpected command received: {}", static_cast<int>(command));
        }
    } catch (const std::exception& e) {
        spdlog::error("Error processing message: {}", e.what());
    }

    return credentials;
}

void LoginProtocol::processMessage(const std::vector<uint8_t>& message) {
    processLoginMessage(message);
}

std::string LoginProtocol::extractString(const std::vector<uint8_t>& message, size_t start) {
    size_t length = 0;
    while (start + length < message.size() && message[start + length] != 0) {
        length++;
    }
    return {reinterpret_cast<const char*>(&message[start]), length};
}
