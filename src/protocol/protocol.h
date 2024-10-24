#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <vector>
#include "protocolenum.h"

class Protocol {
public:
    virtual ~Protocol() = default;

    // Lida com comandos do protocolo de jogo
    virtual void handleProtocolCommand(const std::vector<uint8_t>& message) = 0;

protected:
    // Extrai o comando da mensagem recebida
    static ProtocolCommand extractCommandFromMessage(const std::vector<uint8_t>& message);
};

#endif // PROTOCOL_H
