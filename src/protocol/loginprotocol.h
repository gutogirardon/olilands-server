#ifndef LOGIN_PROTOCOL_H
#define LOGIN_PROTOCOL_H

#include "protocol.h"
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

class LoginProtocol : public Protocol {
public:
    // Processa a mensagem de login
    void processMessage(const std::vector<uint8_t>& message) override;

private:
    // Função auxiliar para extrair uma string de um vetor de bytes
    std::string extractString(const std::vector<uint8_t>& message, size_t start);
};

#endif // LOGIN_PROTOCOL_H
