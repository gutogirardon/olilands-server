#include "loginprotocol.h"

void LoginProtocol::processMessage(const std::vector<uint8_t>& message) {
    try {
        ProtocolCommand command = readCommand(message);

        if (command == ProtocolCommand::LOGIN) {
            std::string username = extractString(message, 1); // Lê o username começando no byte 1
            std::string password = extractString(message, 1 + username.size() + 1); // Lê a senha após o username

            spdlog::info("Login attempt with username: {} and password: {}", username, password);
            // Aqui você pode adicionar a lógica para validar o login
        } else {
            spdlog::error("Unexpected command received: {}", static_cast<int>(command));
        }
    } catch (const std::exception& e) {
        spdlog::error("Error processing message: {}", e.what());
    }
}

std::string LoginProtocol::extractString(const std::vector<uint8_t>& message, size_t start) {
    size_t length = 0;
    while (start + length < message.size() && message[start + length] != 0) {
        length++;
    }
    return std::string(reinterpret_cast<const char*>(&message[start]), length);
}
