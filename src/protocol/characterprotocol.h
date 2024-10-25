#ifndef CHARACTER_PROTOCOL_H
#define CHARACTER_PROTOCOL_H

#include "protocol.h"
#include "protocolenum.h"
#include <vector>
#include <string>
#include "models/characterinfo.h"
#include "models/charactercreationinfo.h"

class CharacterProtocol : public Protocol {
public:
    CharacterProtocol() = default;

    // Lida com a solicitação da lista de personagens
    void handleCharacterListRequest(const std::vector<uint8_t>& message);

    // Lida com a criação de um personagem
    CharacterCreationInfo handleCharacterCreationRequest(const std::vector<uint8_t>& message);

    // Lida com a seleção de um personagem
    CharacterInfo handleCharacterSelectionRequest(const std::vector<uint8_t>& message);

    // Extrai o comando da mensagem
    ProtocolCommand getCommandFromMessage(const std::vector<uint8_t>& message);

    // Lida com qualquer comando relacionado a personagens
    void handleProtocolCommand(const std::vector<uint8_t>& message) override;

private:
    // Função auxiliar para extrair strings da mensagem
    static std::string extractDataString(const std::vector<uint8_t>& message, size_t start);

    // Função auxiliar para extrair inteiros da mensagem
    static int extractDataInt(const std::vector<uint8_t>& message, size_t start);
};

#endif // CHARACTER_PROTOCOL_H
