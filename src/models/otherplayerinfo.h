// models/otherplayerinfo.h
#ifndef OTHER_PLAYER_INFO_H
#define OTHER_PLAYER_INFO_H

#include <string>

struct OtherPlayerInfo {
    int playerId;          // ID do jogador
    std::string name;      // Nome do jogador
    float positionX;       // Posição X
    float positionY;       // Posição Y
    float positionZ;       // Posição Z
    uint8_t state;         // Estado do jogador (0: Idle, 1: Moving, etc.)
};

#endif // OTHER_PLAYER_INFO_H
