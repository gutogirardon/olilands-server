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
    // Espera uma mensagem com pelo menos 3 bytes: comando + x + y
    if (message.size() < 3) {
        throw std::runtime_error("Insufficient message size for movement data");
    }

    // Extraindo o deslocamento x e y
    int x = static_cast<int8_t>(message[1]);  // Interpreta como número com sinal (-128 a 127)
    int y = static_cast<int8_t>(message[2]);

    spdlog::info("Extracted movement coordinates: x = {}, y = {}", x, y);
    return {x, y};
}

std::vector<uint8_t> MovementProtocol::createMovementMessage(ProtocolCommand command, int x, int y) {
    std::vector<uint8_t> message(3);
    message[0] = static_cast<uint8_t>(command);
    message[1] = static_cast<uint8_t>(x);  // Converte para uint8, pois vamos enviar 1 byte
    message[2] = static_cast<uint8_t>(y);

    spdlog::info("Created movement message: Command = {}, x = {}, y = {}", static_cast<int>(command), x, y);
    return message;
}

std::vector<uint8_t> MovementProtocol::createPositionUpdateMessage(int playerId, int x, int y) {
    std::vector<uint8_t> message(7);
    message[0] = static_cast<uint8_t>(ProtocolCommand::UPDATE_POSITION);
    message[1] = (playerId >> 8) & 0xFF;
    message[2] = playerId & 0xFF;
    message[3] = (x >> 8) & 0xFF;
    message[4] = x & 0xFF;
    message[5] = (y >> 8) & 0xFF;
    message[6] = y & 0xFF;

    spdlog::info("Created position update message for playerId: {}, x: {}, y: {}", playerId, x, y);
    return message;
}
