// src/protocol/protocolenum.h
#ifndef PROTOCOL_ENUM_H
#define PROTOCOL_ENUM_H

#include <cstdint>

enum class ProtocolCommand : uint8_t {
    LOGIN = 1,      // Comando para login
    LOGOUT = 2,     // Comando para logout
    PING = 3,       // Comando para ping
};

#endif // PROTOCOL_ENUM_H
