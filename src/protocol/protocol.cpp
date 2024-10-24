#include "protocol.h"
#include <stdexcept>

ProtocolCommand Protocol::extractCommandFromMessage(const std::vector<uint8_t>& message) {
    if (message.empty()) {
        throw std::runtime_error("Empty message");
    }
    return static_cast<ProtocolCommand>(message[0]); // Extrai o comando do primeiro byte da mensagem
}
