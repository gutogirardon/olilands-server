// attackprotocol.h
#ifndef ATTACKPROTOCOL_H
#define ATTACKPROTOCOL_H

#include <vector>
#include <cstdint>
#include "protocolenum.h"

class AttackProtocol {
public:
    AttackProtocol();

    // Métodos para processar mensagens de ataque
    ProtocolCommand getAttackCommandFromMessage(const std::vector<uint8_t>& message);
    std::tuple<int, int, uint8_t> extractAttackData(const std::vector<uint8_t>& message);
    std::vector<uint8_t> createAttackMessage(int attackerId, int targetId, uint8_t attackType);
    std::vector<uint8_t> createAttackResponseMessage(ProtocolCommand responseCommand, int attackerId, int targetId, uint16_t damage = 0);
};

#endif // ATTACKPROTOCOL_H
