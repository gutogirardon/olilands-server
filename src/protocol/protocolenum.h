#ifndef PROTOCOL_ENUM_H
#define PROTOCOL_ENUM_H

#include <cstdint>

enum class ProtocolCommand : uint8_t {
    LOGIN = 1,                // Comando para login
    LOGOUT = 2,               // Comando para logout
    PING = 3,                 // Comando para ping
    CREATE_CHARACTER = 4,     // Comando para criação de personagem
    REQUEST_CHARACTER_LIST = 5,  // Novo comando para solicitar a lista de personagens
    SELECT_CHARACTER = 6,         // Novo comando para selecionar um personagem
    MOVE_CHARACTER = 7,       // Novo comando para movimentação do personagem
    UPDATE_POSITION = 8       // Novo comando para atualizar a posição de um personagem
};

#endif // PROTOCOL_ENUM_H
