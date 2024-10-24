#ifndef LOGIN_PROTOCOL_H
#define LOGIN_PROTOCOL_H

#include "protocol.h"
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

struct PlayerLoginInfo {
    std::string username;
    std::string password;
};

class LoginProtocol : public Protocol {
public:
    LoginProtocol() = default;

    // Lida com a requisição de login e retorna as informações de login do jogador
    PlayerLoginInfo handleLoginRequest(const std::vector<uint8_t>& message);

    // Lida com qualquer mensagem de protocolo do jogo
    void handleProtocolCommand(const std::vector<uint8_t>& message) override;

private:
    // Função auxiliar para extrair uma string de dados de uma mensagem
    static std::string extractDataString(const std::vector<uint8_t>& message, size_t start);
};

#endif // LOGIN_PROTOCOL_H