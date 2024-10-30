#ifndef MOVEMENTPROTOCOL_H
#define MOVEMENTPROTOCOL_H

#include <vector>
#include <cstdint>
#include "protocolenum.h"

class MovementProtocol {
public:
    MovementProtocol();

    ProtocolCommand getCommandFromMessage(const std::vector<uint8_t>& message);
    std::pair<int, int> extractMovementData(const std::vector<uint8_t>& message);
    std::vector<uint8_t> createMovementMessage(ProtocolCommand command, int x, int y);
    std::vector<uint8_t> createPositionUpdateMessage(int playerId, int x, int y);
};

#endif // MOVEMENTPROTOCOL_H
