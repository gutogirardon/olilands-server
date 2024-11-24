// attackprotocol.cpp
#include "attackprotocol.h"
#include <stdexcept>
#include <spdlog/spdlog.h>

AttackProtocol::AttackProtocol() = default;

// Obtém o comando de ataque a partir da mensagem recebida
ProtocolCommand AttackProtocol::getAttackCommandFromMessage(const std::vector<uint8_t>& message) {
    if (message.empty()) {
        throw std::runtime_error("Empty attack message");
    }
    return static_cast<ProtocolCommand>(message[0]);
}

// Extrai os dados de ataque da mensagem
std::tuple<int, int, uint8_t> AttackProtocol::extractAttackData(const std::vector<uint8_t>& message) {
    if (message.size() < 6) {
        throw std::runtime_error("Insufficient message size for attack data");
    }

    int attackerId = (static_cast<uint16_t>(message[1]) << 8) | static_cast<uint16_t>(message[2]);
    int targetId = (static_cast<uint16_t>(message[3]) << 8) | static_cast<uint16_t>(message[4]);
    uint8_t attackType = message[5];

    spdlog::info("Extracted attack data: attackerId = {}, targetId = {}, attackType = {}", attackerId, targetId, attackType);
    return {attackerId, targetId, attackType};
}

// Cria uma mensagem de ataque para ser enviada
std::vector<uint8_t> AttackProtocol::createAttackMessage(int attackerId, int targetId, uint8_t attackType) {
    std::vector<uint8_t> message(6);
    message[0] = static_cast<uint8_t>(ProtocolCommand::ATTACK);
    message[1] = (attackerId >> 8) & 0xFF; // Byte alto do attackerId
    message[2] = attackerId & 0xFF;        // Byte baixo do attackerId
    message[3] = (targetId >> 8) & 0xFF;   // Byte alto do targetId
    message[4] = targetId & 0xFF;          // Byte baixo do targetId
    message[5] = attackType;               // Tipo de ataque

    spdlog::info("Created attack message: attackerId = {}, targetId = {}, attackType = {}", attackerId, targetId, attackType);
    return message;
}

// Cria uma mensagem de resposta para um ataque (sucesso ou falha)
std::vector<uint8_t> AttackProtocol::createAttackResponseMessage(ProtocolCommand responseCommand, int attackerId, int targetId, uint16_t damage) {
    if (responseCommand != ProtocolCommand::ATTACK_SUCCESS && responseCommand != ProtocolCommand::ATTACK_FAILURE) {
        throw std::invalid_argument("Invalid response command for attack");
    }

    std::vector<uint8_t> message;

    if (responseCommand == ProtocolCommand::ATTACK_SUCCESS) {
        // Para sucesso, incluímos o dano
        message.resize(7);
        message[0] = static_cast<uint8_t>(ProtocolCommand::ATTACK_SUCCESS);
        message[1] = (attackerId >> 8) & 0xFF;
        message[2] = attackerId & 0xFF;
        message[3] = (targetId >> 8) & 0xFF;
        message[4] = targetId & 0xFF;
        message[5] = (damage >> 8) & 0xFF;
        message[6] = damage & 0xFF;

        spdlog::info("Created attack success message: attackerId = {}, targetId = {}, damage = {}", attackerId, targetId, damage);
    } else { // ATTACK_FAILURE
        // Para falha, não incluímos o dano
        message.resize(5);
        message[0] = static_cast<uint8_t>(ProtocolCommand::ATTACK_FAILURE);
        message[1] = (attackerId >> 8) & 0xFF;
        message[2] = attackerId & 0xFF;
        message[3] = (targetId >> 8) & 0xFF;
        message[4] = targetId & 0xFF;

        spdlog::info("Created attack failure message: attackerId = {}, targetId = {}", attackerId, targetId);
    }

    return message;
}
