// src/protocol/protocol.h
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <vector>
#include "protocolenum.h"

class Protocol {
public:
    virtual ~Protocol() = default;

    // Função principal para processar pacotes recebidos
    virtual void processMessage(const std::vector<uint8_t>& message) = 0;

protected:
    // Função auxiliar para ler o comando de um pacote
    ProtocolCommand readCommand(const std::vector<uint8_t>& message) {
        if (message.empty()) {
            throw std::runtime_error("Empty message");
        }
        return static_cast<ProtocolCommand>(message[0]);
    }
};

#endif // PROTOCOL_H
