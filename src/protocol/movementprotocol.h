#ifndef MOVEMENTPROTOCOL_H
#define MOVEMENTPROTOCOL_H

#include <vector>
#include <cstdint>
#include "protocolenum.h"

class MovementProtocol {
public:
    MovementProtocol();

    ProtocolCommand getCommandFromMessage(const std::vector<uint8_t>& message);

    // Extrai dados de movimento, agora como uint16_t para suportar valores maiores em pixels
    std::pair<int, int> extractMovementData(const std::vector<uint8_t>& message);

    // Cria uma mensagem de movimento usando uint16_t para posições em pixels
    std::vector<uint8_t> createMovementMessage(ProtocolCommand command, int x, int y);

    // Cria uma mensagem de atualização de posição usando uint16_t para posições em pixels
    std::vector<uint8_t> createPositionUpdateMessage(int playerId, int x, int y);
};

#endif // MOVEMENTPROTOCOL_H
