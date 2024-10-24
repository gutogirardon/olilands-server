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

struct PlayerCreationInfo {
    std::string name;
    std::string vocation;
};

class LoginProtocol : public Protocol {
public:
    LoginProtocol() = default;

    // Lida com a requisição de login e retorna as informações de login do jogador
    PlayerLoginInfo handleLoginRequest(const std::vector<uint8_t>& message);

    // Lida com a criação de personagem
    PlayerCreationInfo handleCharacterCreationRequest(const std::vector<uint8_t>& message);

    // Lida com qualquer mensagem de protocolo do jogo
    void handleProtocolCommand(const std::vector<uint8_t>& message) override;

    ProtocolCommand getCommandFromMessage(const std::vector<uint8_t>& message);

private:
    // Função auxiliar para extrair uma string de dados de uma mensagem
    static std::string extractDataString(const std::vector<uint8_t>& message, size_t start);
};

#endif // LOGIN_PROTOCOL_H
