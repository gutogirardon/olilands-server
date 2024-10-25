#ifndef CHARACTER_INFO_H
#define CHARACTER_INFO_H

#include <string>

struct CharacterInfo {
    int id;                     // ID do personagem
    std::string name;           // Nome do personagem
    std::string vocation;       // Vocação do personagem
    int account_id;             // ID da conta do jogador
    int health;                 // Vida atual
    int max_health;             // Vida máxima
    int mana;                   // Mana atual
    int max_mana;               // Mana máxima
    int look_body;              // Aparência do corpo
    int look_feet;              // Aparência dos pés
    int look_head;              // Aparência da cabeça
    int look_legs;              // Aparência das pernas
    int look_type;              // Aparência do tipo de personagem
    int magic_level;            // Nível mágico do personagem
    int level;                  // Nível do personagem
    long long experience;       // Experiência acumulada
    int pos_x;                  // Posição X do personagem
    int pos_y;                  // Posição Y do personagem
    int pos_z;                  // Posição Z do personagem
    std::string gender;         // Gênero do personagem (masculino ou feminino)
    bool blessed;               // Status de bênção
    double balance;             // Saldo de dinheiro do personagem
    int hours_played;           // Horas jogadas pelo personagem
};

#endif // CHARACTER_INFO_H
