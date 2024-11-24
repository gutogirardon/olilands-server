// protocolenum.h
#ifndef PROTOCOL_ENUM_H
#define PROTOCOL_ENUM_H

#include <cstdint>

enum class ProtocolCommand : uint8_t {
    LOGIN = 1,                      // Comando para login
    LOGOUT = 2,                     // Comando para logout
    PING = 3,                       // Comando para ping
    CREATE_CHARACTER = 4,           // Comando para criação de personagem
    REQUEST_CHARACTER_LIST = 5,     // Comando para solicitar a lista de personagens
    SELECT_CHARACTER = 6,           // Comando para selecionar um personagem
    MOVE_CHARACTER = 7,             // Comando para movimentação do personagem
    UPDATE_POSITION = 8,            // Comando para atualizar a posição de um personagem
    ATTACK = 9,                     // Comando para realizar um ataque


    // Comandos de resposta
    LOGIN_SUCCESS = 101,            // Resposta de login bem-sucedido
    LOGIN_FAILURE = 102,            // Resposta de login falhado
    CHARACTER_LIST = 103,           // Resposta com a lista de personagens
    CHARACTER_SELECTION_SUCCESS = 104, // Resposta de seleção de personagem bem-sucedida
    CHARACTER_SELECTION_FAILURE = 105, // Resposta de seleção de personagem falhada
    CHARACTER_CREATION_SUCCESS = 106,  // Resposta de criação de personagem bem-sucedida
    CHARACTER_CREATION_FAILURE = 107,  // Resposta de criação de personagem falhada
    MOVEMENT_CONFIRMED = 108,           // Resposta de movimento confirmado
    MOVEMENT_BLOCKED = 109,             // Resposta de movimento bloqueado
    PONG = 110,                         // Resposta de pong

    ATTACK_SUCCESS = 111,               // Resposta de ataque bem-sucedido
    ATTACK_FAILURE = 112,               // Resposta de ataque falhado
    ATTACK_NOTIFICATION = 113,          // Notificação de ataque realizado a outros jogadores

    OTHER_PLAYER_INFO = 201,            // Informações iniciais de outros jogadores
    OTHER_PLAYER_UPDATE = 202,          // Atualizações de posição de outros jogadores
    OTHER_PLAYER_DISCONNECTED = 203     // Notificação de desconexão de outros jogadores
};

#endif // PROTOCOL_ENUM_H
