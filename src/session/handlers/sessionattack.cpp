#include "sessionattack.h"
#include "session/session.h"
#include "database/databasemanager.h"
#include "world/world.h"
#include "database/playermanager.h"
#include <spdlog/spdlog.h>
#include <exception>

SessionAttack::SessionAttack(Session& session)
    : session_(session), attackProtocol_()
{}

void SessionAttack::handleAttackCommands(const std::vector<uint8_t>& message) {
    try {
        auto [attackerId, targetId, attackType] = attackProtocol_.extractAttackData(message);

        if (attackerId != session_.getPlayerId()) {
            spdlog::warn("O jogador {} está tentando atacar como {}", session_.getPlayerId(), attackerId);
            std::vector<uint8_t> errorMessage = attackProtocol_.createAttackResponseMessage(ProtocolCommand::ATTACK_FAILURE, attackerId, targetId, 0);
            session_.sendDataToClient(errorMessage);
            return;
        }

        // TODO: Implementar a lógica real de ataque aqui
        // Por exemplo, verificar se o alvo está próximo, calcular dano, etc.

        // Mock: Retorna sucesso sempre com dano fixo
        uint16_t damage = 10;
        ProtocolCommand responseCmd = ProtocolCommand::ATTACK_SUCCESS; // Pode ser ATTACK_FAILURE dependendo da lógica
        std::vector<uint8_t> responseMessage = attackProtocol_.createAttackResponseMessage(responseCmd, attackerId, targetId, damage);

        session_.sendDataToClient(responseMessage);

        // TODO: Notificar outros jogadores ou atualizar o estado no mundo
        // Por exemplo:
        // session_.getWorld().notifyAttack(attackerId, targetId, damage);

    } catch (const std::exception& ex) {
        spdlog::error("Erro ao lidar com comandos de ataque: {}", ex.what());
        std::vector<uint8_t> errorMessage = attackProtocol_.createAttackResponseMessage(ProtocolCommand::ATTACK_FAILURE, 0, 0, 0);
        session_.sendDataToClient(errorMessage);
    }

    // Continua recebendo dados do cliente após o ataque
    session_.receiveClientData();
}
