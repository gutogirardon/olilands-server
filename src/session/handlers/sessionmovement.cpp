#include "sessionmovement.h"
#include "session/session.h"
#include "database/databasemanager.h"
#include "world/world.h"
#include "session/sessionmanager.h"
#include <spdlog/spdlog.h>
#include <exception>

SessionMovement::SessionMovement(Session& session, DatabaseManager& dbManager, World& world, SessionManager& sessionManager)
    : session_(session), dbManager_(dbManager), world_(world), sessionManager_(sessionManager), movementProtocol_()
{}

void SessionMovement::handleMovementCommands(const std::vector<uint8_t>& message) {
    try {
        ProtocolCommand command = movementProtocol_.getCommandFromMessage(message);

        if (command == ProtocolCommand::MOVE_CHARACTER) {
            auto [new_x, new_y] = movementProtocol_.extractMovementData(message);

            // Verifica se a nova posição é caminhável
            if (world_.isPositionWalkable(new_x, new_y)) {
                // Atualiza a posição do jogador no mundo
                world_.updatePlayerPosition(session_.getPlayerId(), new_x, new_y);

                // Obtém a nova posição atualizada
                auto [updated_x, updated_y, updated_z] = world_.getPlayerPosition(session_.getPlayerId());

                // Cria a mensagem de confirmação de movimento
                std::vector<uint8_t> movementConfirmedMessage = {static_cast<uint8_t>(ProtocolCommand::MOVEMENT_CONFIRMED)};
                // Serializa updated_x, updated_y, updated_z como float (4 bytes cada)
                float posX = static_cast<float>(updated_x);
                float posY = static_cast<float>(updated_y);
                float posZ = static_cast<float>(updated_z);
                uint8_t* posXBytes = reinterpret_cast<uint8_t*>(&posX);
                uint8_t* posYBytes = reinterpret_cast<uint8_t*>(&posY);
                uint8_t* posZBytes = reinterpret_cast<uint8_t*>(&posZ);
                movementConfirmedMessage.insert(movementConfirmedMessage.end(), posXBytes, posXBytes + sizeof(float));
                movementConfirmedMessage.insert(movementConfirmedMessage.end(), posYBytes, posYBytes + sizeof(float));
                movementConfirmedMessage.insert(movementConfirmedMessage.end(), posZBytes, posZBytes + sizeof(float));

                // Envia a confirmação de movimento ao cliente
                session_.sendDataToClient(movementConfirmedMessage);

                // Define o range de proximidade
                int range = 500;

                // Envia a atualização de posição para jogadores próximos
                std::vector<int> nearbyPlayers = world_.getPlayersInProximity(session_.getPlayerId(), range);
                auto positionUpdateMessage = movementProtocol_.createPositionUpdateMessage(session_.getPlayerId(), updated_x, updated_y);

                for (int nearbyPlayerId : nearbyPlayers) {
                    if (nearbyPlayerId == session_.getPlayerId()) continue; // Evita enviar para si mesmo

                    auto sessionPtr = sessionManager_.getSession(nearbyPlayerId);
                    if (sessionPtr) {
                        // **Otimização: Evitar Cálculo de Distância Dupla**
                        // Supondo que `getPlayersInProximity` já filtrou por range
                        sessionPtr->sendDataToClient(positionUpdateMessage);
                        spdlog::debug("Sent OTHER_PLAYER_UPDATE to playerId {}", nearbyPlayerId);
                    } else {
                        spdlog::debug("No session found for playerId {}", nearbyPlayerId);
                    }
                }

                spdlog::info("Player {} moved to x = {}, y = {} and update sent to nearby players",
                             session_.getPlayerId(), new_x, new_y);
            } else {
                spdlog::warn("Player {} attempted to move to invalid position ({}, {})",
                             session_.getPlayerId(), new_x, new_y);
                // Crie a resposta binária de movimento bloqueado
                std::vector<uint8_t> movementBlockedMessage = {static_cast<uint8_t>(ProtocolCommand::MOVEMENT_BLOCKED)};
                session_.sendDataToClient(movementBlockedMessage);
            }

            // Continua recebendo dados do cliente após o movimento
            session_.receiveClientData();
        } else {
            spdlog::error("Received invalid movement command: {}", static_cast<int>(command));
            session_.receiveClientData();
        }
    } catch (const std::exception& ex) {
        spdlog::error("Erro ao lidar com comandos de movimento: {}", ex.what());
        // Opcional: Envie uma mensagem de erro ao cliente, se apropriado
        session_.receiveClientData();
    }
}
