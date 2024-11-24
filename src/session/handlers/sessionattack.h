#ifndef SESSIONATTACK_H
#define SESSIONATTACK_H

#include <vector>
#include "protocol/attackprotocol.h"

class Session; // Forward declaration

class SessionAttack {
public:
    // Construtor que recebe referência para a sessão
    SessionAttack(Session& session);

    // Método para lidar com comandos de ataque
    void handleAttackCommands(const std::vector<uint8_t>& message);

private:
    Session& session_;
    AttackProtocol attackProtocol_;
};

#endif // SESSIONATTACK_H
