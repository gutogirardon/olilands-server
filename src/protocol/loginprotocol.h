#ifndef LOGIN_PROTOCOL_H
#define LOGIN_PROTOCOL_H

#include "protocol.h"
#include "protocolenum.h"

#include <spdlog/spdlog.h>
#include <string>
#include <vector>
#include <cstdint>

struct PlayerLoginInfo {
    std::string username;
    std::string password;
};

struct LoginSuccessResponse {
    ProtocolCommand command = ProtocolCommand::LOGIN_SUCCESS;
    uint16_t accountId; // Exemplo de dado adicional
};

struct LoginFailureResponse {
    ProtocolCommand command = ProtocolCommand::LOGIN_FAILURE;
    uint8_t errorCode; // Código de erro específico
};

class LoginProtocol : public Protocol {
public:
    LoginProtocol() = default;

    // Lida com a requisição de login e retorna as informações de login do jogador
    PlayerLoginInfo handleLoginRequest(const std::vector<uint8_t>& message);

    // Lida com qualquer mensagem de protocolo do jogo
    void handleProtocolCommand(const std::vector<uint8_t>& message) override;

    ProtocolCommand getCommandFromMessage(const std::vector<uint8_t>& message);

    // Funções para criar respostas binárias
    std::vector<uint8_t> createLoginSuccess(uint16_t accountId);
    std::vector<uint8_t> createLoginFailure(uint8_t errorCode);

private:
    // Função auxiliar para extrair uma string de dados de uma mensagem
    static std::string extractDataString(const std::vector<uint8_t>& message, size_t start);
};

#endif // LOGIN_PROTOCOL_H
