#include "session.h"
#include "sessionmanager.h"
#include "models/characterinfo.h"
#include "models/otherplayerinfo.h"
#include "protocol/loginprotocol.h"
#include "protocol/movementprotocol.h"
#include "protocol/characterprotocol.h"
#include "database/useraccountmanager.h"
#include "database/playermanager.h"
#include <vector>
#include <spdlog/spdlog.h>
#include <exception>

Session::Session(boost::asio::ip::tcp::socket socket, DatabaseManager& dbManager, World& world, SessionManager& sessionManager)
    : socket_(std::move(socket)),
      dbManager_(dbManager),
      world_(world),
      sessionManager_(sessionManager),
      login_timer_(socket_.get_executor()),
      position_update_timer_(socket_.get_executor()),
      last_saved_x_(0), last_saved_y_(0), last_saved_z_(0),
      attackHandler_(*this),
      movementHandler_(*this, dbManager, world, sessionManager)
{}

void Session::beginSession() {
    try {
        spdlog::info("Client connected: {}:{}",
                     socket_.remote_endpoint().address().to_string(),
                     socket_.remote_endpoint().port());

        login_timer_.expires_after(std::chrono::seconds(3));
        auto self = shared_from_this();
        login_timer_.async_wait([self](const boost::system::error_code& ec) {
            if (!ec) {
                self->handleLoginTimeout();
            }
        });

        receiveClientData();
    }
    catch (const std::exception& e) {
        spdlog::error("Error: {}", e.what());
    }
}

void Session::startPositionUpdateTimer() {
    if (!socket_.is_open()) return;

    position_update_timer_.expires_after(std::chrono::milliseconds(500));
    auto self = shared_from_this();
    position_update_timer_.async_wait([this, self](const boost::system::error_code& ec) {
        if (!ec) {
            savePlayerPosition();
            startPositionUpdateTimer();
        }
    });
}

void Session::savePlayerPosition() {
    if (!is_position_valid || !socket_.is_open()) return;

    auto [x, y, z] = world_.getPlayerPosition(player_id_);

    // Só salva se a posição tiver mudado desde a última atualização
    if (x == last_saved_x_ && y == last_saved_y_ && z == last_saved_z_) return;

    PlayerManager playerManager(dbManager_);
    if (playerManager.updatePlayerPosition(player_id_, x, y, z)) {
        spdlog::info("Player {} position saved to database: ({}, {}, {})", player_id_, x, y, z);

        // Atualiza os valores salvos
        last_saved_x_ = x;
        last_saved_y_ = y;
        last_saved_z_ = z;
    } else {
        spdlog::error("Failed to save player {} position to database.", player_id_);
    }
}

void Session::receiveClientData() {
    auto self = shared_from_this();
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                login_timer_.cancel();
                std::vector<uint8_t> message(data_, data_ + length);

                switch (player_session_state_) {
                    case State::Unauthenticated:
                        authenticatePlayer(message);
                        break;
                    case State::Authenticated:
                        handleCharacterSelectionCommands(message);
                        break;
                    case State::InGame:
                        handlePlayerCommands(message);
                        break;
                    default:
                        spdlog::error("Invalid session state.");
                        socket_.close();
                        break;
                }
            } else {
                handleDisconnection(ec);
            }
        });
}

void Session::handleDisconnection(const boost::system::error_code& ec) {
    if (ec == boost::asio::error::operation_aborted) {
        spdlog::debug("Operation canceled as expected.");
    } else if (ec == boost::asio::error::eof) {
        spdlog::info("Client disconnected");
        savePlayerPosition();  // Salva posição final ao desconectar
        position_update_timer_.cancel();  // Para o timer
    } else {
        spdlog::error("Error on read: {}", ec.message());
    }

    if (player_session_state_ == State::InGame) {
        sessionManager_.removeSession(player_id_);
        spdlog::info("Session: Removed session from SessionManager for player_id_ {}", player_id_);
    }

    socket_.close();
}

void Session::handleCharacterSelectionCommands(const std::vector<uint8_t>& message) {
    CharacterProtocol characterProtocol;
    ProtocolCommand command = characterProtocol.getCommandFromMessage(message);

    switch (command) {
        case ProtocolCommand::REQUEST_CHARACTER_LIST: {
            PlayerManager playerManager(dbManager_);
            std::vector<CharacterInfo> characters = playerManager.getCharactersForAccount(account_id_);
            std::vector<uint8_t> characterListMessage = characterProtocol.createCharacterList(characters);
            sendDataToClient(characterListMessage);
            receiveClientData();
            break;
        }
        case ProtocolCommand::SELECT_CHARACTER: {
            CharacterInfo selectedCharacter = characterProtocol.handleCharacterSelectionRequest(message);
            PlayerManager playerManager(dbManager_);
            CharacterInfo characterInfo = playerManager.getCharacterInfo(selectedCharacter.name, account_id_);

            if (!characterInfo.name.empty()) {
                spdlog::info("Character {} selected by user {}", characterInfo.name, username_);

                // Obter a posição do jogador antes de definir player_id_
                auto [pos_x, pos_y, pos_z] = playerManager.getPlayerPosition(characterInfo.id);

                if (pos_x != 0 || pos_y != 0 || pos_z != 0) {
                    is_position_valid = true;
                    world_.updatePlayerPosition(characterInfo.id, pos_x, pos_y);

                    // Definir player_id_ antes de adicionar a sessão
                    player_id_ = characterInfo.id;
                    spdlog::info("Session: player_id_ set to {}", player_id_);

                    // Adicionar a sessão ao SessionManager com player_id_ correto
                    sessionManager_.addSession(player_id_, shared_from_this());
                    spdlog::info("Session: Registered session in SessionManager for player_id_ {}", player_id_);

                    player_session_state_ = State::InGame;

                    // Inicia o temporizador para atualização de posição no banco
                    startPositionUpdateTimer();

                    // **Enviar Informações de Jogadores Próximos**
                    sendNearbyPlayersInfo();

                    // Crie a resposta binária de seleção bem-sucedida com a posição do jogador
                    std::vector<uint8_t> selectionSuccessMessage = characterProtocol.createCharacterSelectionSuccess(characterInfo.id, pos_x, pos_y, pos_z);
                    sendDataToClient(selectionSuccessMessage);
                } else {
                    spdlog::warn("Invalid position for character {}: ({}, {}, {})", characterInfo.name, pos_x, pos_y, pos_z);

                    // Crie a resposta binária de seleção falhada
                    std::vector<uint8_t> selectionFailureMessage = characterProtocol.createCharacterSelectionFailure(2); // Código de erro
                    sendDataToClient(selectionFailureMessage);
                }
                receiveClientData();
            } else {
                spdlog::error("Character {} not found for account {}", selectedCharacter.name, account_id_);

                // Crie a resposta binária de seleção falhada
                std::vector<uint8_t> selectionFailureMessage = characterProtocol.createCharacterSelectionFailure(1); // Código de erro
                sendDataToClient(selectionFailureMessage);
                receiveClientData();
            }
            break;
        }
        case ProtocolCommand::CREATE_CHARACTER: {
            CharacterCreationInfo creationInfo = characterProtocol.handleCharacterCreationRequest(message);
            PlayerManager playerManager(dbManager_);
            if (playerManager.createCharacter(creationInfo.name, creationInfo.vocation, account_id_)) {
                spdlog::info("Character created successfully.");

                // Obtenha o ID do novo personagem criado
                CharacterInfo newCharacter = playerManager.getCharacterInfo(creationInfo.name, account_id_);

                // Crie a resposta binária de criação bem-sucedida
                std::vector<uint8_t> creationSuccessMessage = characterProtocol.createCharacterCreationSuccess(newCharacter.id);
                sendDataToClient(creationSuccessMessage);
            } else {
                spdlog::error("Character creation failed.");

                // Crie a resposta binária de criação falhada
                std::vector<uint8_t> creationFailureMessage = characterProtocol.createCharacterCreationFailure(1); // Código de erro
                sendDataToClient(creationFailureMessage);
            }
            receiveClientData();
            break;
        }
        default:
            spdlog::error("Unknown character selection command received: {}", static_cast<int>(command));
            receiveClientData();
            break;
    }
}

std::string Session::getPlayerName(int playerId) const {
    PlayerManager playerManager(dbManager_);
    try {
        CharacterInfo characterInfo = playerManager.getCharacterInfoById(playerId);
        return characterInfo.name;
    } catch (const std::exception& e) {
        spdlog::error("Failed to get player name for playerId {}: {}", playerId, e.what());
        return "Unknown";
    }
}

uint8_t Session::getPlayerState(int playerId) const {
    return 0; //@todo retornar o estado do jogador aqui
}

std::vector<uint8_t> Session::createOtherPlayerInfoMessage(const OtherPlayerInfo& info) {
    std::vector<uint8_t> data;
    data.push_back(static_cast<uint8_t>(ProtocolCommand::OTHER_PLAYER_INFO));

    // playerId (4 bytes)
    data.push_back((info.playerId >> 24) & 0xFF);
    data.push_back((info.playerId >> 16) & 0xFF);
    data.push_back((info.playerId >> 8) & 0xFF);
    data.push_back(info.playerId & 0xFF);

    // name (32 bytes, null-terminated)
    for(int i = 0; i < 32; ++i){
        if(i < info.name.size())
            data.push_back(static_cast<uint8_t>(info.name[i]));
        else
            data.push_back(0);
    }

    // positionX (4 bytes float)
    float posX = info.positionX;
    uint8_t* posXBytes = reinterpret_cast<uint8_t*>(&posX);
    data.insert(data.end(), posXBytes, posXBytes + sizeof(float));

    // positionY (4 bytes float)
    float posY = info.positionY;
    uint8_t* posYBytes = reinterpret_cast<uint8_t*>(&posY);
    data.insert(data.end(), posYBytes, posYBytes + sizeof(float));

    // positionZ (4 bytes float)
    float posZ = info.positionZ;
    uint8_t* posZBytes = reinterpret_cast<uint8_t*>(&posZ);
    data.insert(data.end(), posZBytes, posZBytes + sizeof(float));

    // state (1 byte)
    data.push_back(info.state);

    spdlog::info("Created OTHER_PLAYER_INFO message for playerId {}", info.playerId);
    return data;
}

void Session::sendNearbyPlayersInfo() {
    if (!is_position_valid) return;

    int range = 500;
    std::vector<int> nearbyPlayers = world_.getPlayersInProximity(player_id_, range);

    PlayerManager playerManager(dbManager_);

    for (int otherPlayerId : nearbyPlayers) {
        if (otherPlayerId == player_id_) continue; // Evitar enviar para si mesmo

        try {
            CharacterInfo characterInfo = playerManager.getCharacterInfoById(otherPlayerId);
            auto [x, y, z] = world_.getPlayerPosition(otherPlayerId);
            uint8_t state = getPlayerState(otherPlayerId);

            OtherPlayerInfo info;
            info.playerId = otherPlayerId;
            info.name = characterInfo.name;
            info.positionX = static_cast<float>(x);
            info.positionY = static_cast<float>(y);
            info.positionZ = static_cast<float>(z);
            info.state = state;

            std::vector<uint8_t> message = createOtherPlayerInfoMessage(info);
            sendDataToClient(message);

            spdlog::debug("Sent OTHER_PLAYER_INFO to playerId {}", player_id_);
        } catch (const std::exception& e) {
            spdlog::error("Error sending OTHER_PLAYER_INFO for playerId {}: {}", otherPlayerId, e.what());
            continue;
        }
    }
    broadcastNewPlayerInfo();
}

void Session::broadcastNewPlayerInfo() {
    OtherPlayerInfo newPlayerInfo;
    newPlayerInfo.playerId = player_id_;
    newPlayerInfo.name = username_;
    auto [x, y, z] = world_.getPlayerPosition(player_id_);
    newPlayerInfo.positionX = static_cast<float>(x);
    newPlayerInfo.positionY = static_cast<float>(y);
    newPlayerInfo.positionZ = static_cast<float>(z);
    newPlayerInfo.state = getPlayerState(player_id_);

    std::vector<uint8_t> message = createOtherPlayerInfoMessage(newPlayerInfo);
    std::vector<std::shared_ptr<Session>> allSessions = sessionManager_.getAllSessions();

    for (auto& session : allSessions) {
        if (session->getPlayerId() == player_id_) continue; // Evitar enviar para si mesmo

        session->sendDataToClient(message);
        spdlog::debug("Broadcasted OTHER_PLAYER_INFO to playerId {}", session->getPlayerId());
    }

    spdlog::info("Broadcasted new player info for playerId {}", player_id_);
}

void Session::handlePlayerCommands(const std::vector<uint8_t>& message) {
    CharacterProtocol characterProtocol;
    ProtocolCommand command = characterProtocol.getCommandFromMessage(message);

    switch (command) {
        case ProtocolCommand::LOGOUT: {
            savePlayerPosition();
            spdlog::info("Player logged out.");
            socket_.close();
            break;
        }
        case ProtocolCommand::PING: {
            std::vector<uint8_t> pongMessage = {static_cast<uint8_t>(ProtocolCommand::PONG)};
            sendDataToClient(pongMessage);
            break;
        }
        case ProtocolCommand::MOVE_CHARACTER: {
            movementHandler_.handleMovementCommands(message); // Delegação para SessionMovement
            break;
        }
        case ProtocolCommand::ATTACK: {
            attackHandler_.handleAttackCommands(message);
            break;
        }
        default: {
            spdlog::error("Unknown command received: {}", static_cast<int>(command));
            receiveClientData();
            break;
        }
    }
}

void Session::handleLoginTimeout() {
    spdlog::warn("Login timeout for client");
    boost::system::error_code ec;
    socket_.close(ec);

    if (ec) {
        spdlog::error("Error closing socket after timeout: {}", ec.message());
    }
}

void Session::authenticatePlayer(const std::vector<uint8_t>& message) {
    LoginProtocol login_protocol;
    PlayerLoginInfo credentials = login_protocol.handleLoginRequest(message);
    UserAccountManager userAccountManager(dbManager_);
    int account_id = 0;

    if (userAccountManager.validateLogin(credentials.username, credentials.password, account_id)) {
        spdlog::info("Player authenticated: {}", credentials.username);
        player_session_state_ = State::Authenticated;
        account_id_ = account_id;
        username_ = credentials.username;

        // Crie a resposta binária de sucesso
        std::vector<uint8_t> successMessage = login_protocol.createLoginSuccess(static_cast<uint16_t>(account_id));
        sendDataToClient(successMessage);
        
        receiveClientData();
    } else {
        spdlog::error("Authentication failed for player: {}", credentials.username);
        
        std::vector<uint8_t> failureMessage = login_protocol.createLoginFailure(1);
        sendDataToClient(failureMessage);
        
        socket_.close();
    }
}

void Session::sendDataToClient(const std::vector<uint8_t>& message) {
    auto self = shared_from_this();
    auto msg = std::make_shared<std::vector<uint8_t>>(message);
    boost::asio::async_write(socket_, boost::asio::buffer(*msg),
        [this, self, msg](boost::system::error_code ec, std::size_t /*length*/) {
            if (ec) {
                spdlog::error("Error on write: {}", ec.message());
            }
        });
}
