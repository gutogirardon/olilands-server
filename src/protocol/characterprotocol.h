#ifndef CHARACTER_PROTOCOL_H
#define CHARACTER_PROTOCOL_H

#include "protocol.h"
#include "protocolenum.h"
#include <vector>
#include <string>
#include "models/characterinfo.h"
#include "models/charactercreationinfo.h"

struct CharacterSelectionSuccessResponse {
    ProtocolCommand command = ProtocolCommand::CHARACTER_SELECTION_SUCCESS;
    uint32_t characterId; // Exemplo de dado adicional
    // Outros dados necessários podem ser adicionados
};

struct CharacterSelectionFailureResponse {
    ProtocolCommand command = ProtocolCommand::CHARACTER_SELECTION_FAILURE;
    uint8_t errorCode; // Código de erro específico
};

struct CharacterCreationSuccessResponse {
    ProtocolCommand command = ProtocolCommand::CHARACTER_CREATION_SUCCESS;
    uint32_t characterId; // Exemplo de dado adicional
};

struct CharacterCreationFailureResponse {
    ProtocolCommand command = ProtocolCommand::CHARACTER_CREATION_FAILURE;
    uint8_t errorCode; // Código de erro específico
};

struct CharacterListResponse {
    ProtocolCommand command = ProtocolCommand::CHARACTER_LIST;
    uint16_t numCharacters;
    std::vector<CharacterInfo> characters;
};

class CharacterProtocol : public Protocol {
public:
    CharacterProtocol() = default;

    // Lida com a solicitação da lista de personagens
    void handleCharacterListRequest(const std::vector<uint8_t>& message);

    // Lida com a criação de um personagem
    CharacterCreationInfo handleCharacterCreationRequest(const std::vector<uint8_t>& message);

    // Lida com a seleção de um personagem
    CharacterInfo handleCharacterSelectionRequest(const std::vector<uint8_t>& message);

    // Lida com o comando de movimento do personagem
    void handleMovementRequest(const std::vector<uint8_t>& message);

    // Extrai o comando da mensagem
    ProtocolCommand getCommandFromMessage(const std::vector<uint8_t>& message);

    // Lida com qualquer comando relacionado a personagens
    void handleProtocolCommand(const std::vector<uint8_t>& message) override;

    // Funções para criar respostas binárias
    std::vector<uint8_t> createCharacterList(const std::vector<CharacterInfo>& characters);
    std::vector<uint8_t> createCharacterSelectionSuccess(uint32_t characterId, int posX, int posY, int posZ);
    std::vector<uint8_t> createCharacterSelectionFailure(uint8_t errorCode);
    std::vector<uint8_t> createCharacterCreationSuccess(uint32_t characterId);
    std::vector<uint8_t> createCharacterCreationFailure(uint8_t errorCode);

private:
    // Função auxiliar para extrair strings da mensagem
    static std::string extractDataString(const std::vector<uint8_t>& message, size_t start);

    // Função auxiliar para extrair inteiros da mensagem
    static int extractDataInt(const std::vector<uint8_t>& message, size_t start);
};

#endif // CHARACTER_PROTOCOL_H
