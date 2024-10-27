#include "movementprotocol.h"
#include <stdexcept>
#include <spdlog/spdlog.h>

MovementProtocol::MovementProtocol() = default;

ProtocolCommand MovementProtocol::getCommandFromMessage(const std::vector<uint8_t>& message) {
    if (message.empty()) {
        throw std::runtime_error("Empty message");
    }
    return static_cast<ProtocolCommand>(message[0]);
}

std::pair<int, int> MovementProtocol::extractMovementData(const std::vector<uint8_t>& message) {
    if (message.size() < 3) {
        throw std::runtime_error("Insufficient message size for movement data");
    }
    int x = 0, y = 0;
    if (message.size() == 3) {
        // Formato antigo: x e y em 1 byte cada (int8_t)
        x = static_cast<int8_t>(message[1]);  // Interpreta como número com sinal (-128 a 127)
        y = static_cast<int8_t>(message[2]);
        spdlog::info("Extracted movement coordinates (legacy format): x = {}, y = {}", x, y);
    } else if (message.size() >= 5) {
        // Novo formato: x e y em 2 bytes cada (uint16_t)
        x = (static_cast<uint16_t>(message[1]) << 8) | static_cast<uint16_t>(message[2]);
        y = (static_cast<uint16_t>(message[3]) << 8) | static_cast<uint16_t>(message[4]);
        spdlog::info("Extracted movement coordinates (new format): x = {}, y = {}", x, y);
    } else {
        throw std::runtime_error("Unsupported message size for movement data");
    }

    return {x, y};
}

std::vector<uint8_t> MovementProtocol::createMovementMessage(ProtocolCommand command, int x, int y) {
    // Cria uma mensagem com 5 bytes: comando + x_high + x_low + y_high + y_low
    std::vector<uint8_t> message(5);
    message[0] = static_cast<uint8_t>(command);
    message[1] = static_cast<uint8_t>((x >> 8) & 0xFF);  // Byte alto de x
    message[2] = static_cast<uint8_t>(x & 0xFF);         // Byte baixo de x
    message[3] = static_cast<uint8_t>((y >> 8) & 0xFF);  // Byte alto de y
    message[4] = static_cast<uint8_t>(y & 0xFF);         // Byte baixo de y

    spdlog::info("Created movement message: Command = {}, x = {}, y = {}", static_cast<int>(command), x, y);
    return message;
}

std::vector<uint8_t> MovementProtocol::createPositionUpdateMessage(int playerId, int x, int y) {
    // Cria uma mensagem de atualização de posição com 7 bytes: comando + playerId (2 bytes) + x (2 bytes) + y (2 bytes)
    std::vector<uint8_t> message(7);
    message[0] = static_cast<uint8_t>(ProtocolCommand::UPDATE_POSITION);
    message[1] = (playerId >> 8) & 0xFF;           // Byte alto do playerId
    message[2] = playerId & 0xFF;                  // Byte baixo do playerId
    message[3] = (x >> 8) & 0xFF;                  // Byte alto de x
    message[4] = x & 0xFF;                         // Byte baixo de x
    message[5] = (y >> 8) & 0xFF;                  // Byte alto de y
    message[6] = y & 0xFF;                         // Byte baixo de y

    spdlog::info("Created position update message for playerId: {}, x: {}, y: {}", playerId, x, y);
    return message;
}
