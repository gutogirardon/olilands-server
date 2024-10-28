#include "session.h"
#include "sessionmanager.h"
#include "models/characterinfo.h"
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
      last_saved_x_(0), last_saved_y_(0), last_saved_z_(0) {}

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
    if (!socket_.is_open()) return;  // Verifica se a conexão está aberta

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
    sessionManager_.removeSession(player_id_);
    socket_.close();
}

void Session::handleCharacterSelectionCommands(const std::vector<uint8_t>& message) {
    CharacterProtocol characterProtocol;
    ProtocolCommand command = characterProtocol.getCommandFromMessage(message);

    switch (command) {
        case ProtocolCommand::REQUEST_CHARACTER_LIST: {
            PlayerManager playerManager(dbManager_);
            std::vector<CharacterInfo> characters = playerManager.getCharactersForAccount(account_id_);
            std::string characterListMessage = "CharacterList|";
            for (const auto& character : characters) {
                characterListMessage += character.name + "," + character.vocation + "," + std::to_string(character.level) + ";";
            }
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
                sessionManager_.addSession(player_id_, shared_from_this());
                auto [pos_x, pos_y, pos_z] = playerManager.getPlayerPosition(characterInfo.id);

                if (pos_x != 0 || pos_y != 0 || pos_z != 0) {
                    is_position_valid = true;
                    world_.updatePlayerPosition(characterInfo.id, pos_x, pos_y);
                    player_id_ = characterInfo.id;
                    player_session_state_ = State::InGame;

                    // Inicia o temporizador para atualização de posição no banco
                    startPositionUpdateTimer();

                    sendDataToClient("CharacterSelected|" + characterInfo.name + "|" +
                                                                    std::to_string(pos_x) + "," +
                                                                    std::to_string(pos_y) + "," +
                                                                    std::to_string(pos_z));

                } else {
                    spdlog::warn("Invalid position for character {}: ({}, {}, {})", characterInfo.name, pos_x, pos_y, pos_z);
                    sendDataToClient("CharacterSelectionFailed");
                }
                receiveClientData();
            } else {
                spdlog::error("Character {} not found for account {}", selectedCharacter.name, account_id_);
                sendDataToClient("CharacterSelectionFailed");
                receiveClientData();
            }
            break;
        }
        case ProtocolCommand::CREATE_CHARACTER: {
            CharacterCreationInfo creationInfo = characterProtocol.handleCharacterCreationRequest(message);
            PlayerManager playerManager(dbManager_);
            if (playerManager.createCharacter(creationInfo.name, creationInfo.vocation, account_id_)) {
                spdlog::info("Character created successfully.");
                sendDataToClient("Character created successfully");
            } else {
                spdlog::error("Character creation failed.");
                sendDataToClient("Character creation failed");
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

void Session::handleMovementCommands(const std::vector<uint8_t>& message) {
    if (!is_position_valid) {
        spdlog::warn("Invalid position, cannot move playerId {}", player_id_);
        return;
    }

    MovementProtocol movementProtocol;
    ProtocolCommand command = movementProtocol.getCommandFromMessage(message);

    if (command == ProtocolCommand::MOVE_CHARACTER) {
        //auto [current_x, current_y, current_z] = world_.getPlayerPosition(player_id_);
        auto [new_x, new_y] = movementProtocol.extractMovementData(message);

        // Verifica se a nova posição é caminhável
        if (world_.isPositionWalkable(new_x, new_y)) {
            // Atualiza a posição do jogador no mundo
            world_.updatePlayerPosition(player_id_, new_x, new_y);

            // Obtém a nova posição atualizada
            auto [updated_x, updated_y, updated_z] = world_.getPlayerPosition(player_id_);

            // Cria a mensagem de confirmação de movimento
            std::string confirmationMessage = "MovementConfirmed|" +
                std::to_string(updated_x) + "," +
                std::to_string(updated_y) + "," +
                std::to_string(updated_z);

            // Envia a confirmação de movimento ao cliente
            sendDataToClient(confirmationMessage);

            // Envia a atualização de posição para jogadores próximos
            std::vector<int> nearbyPlayers = world_.getPlayersInProximity(player_id_, 2000); //@todo ver que tamanho vamos usar
            auto positionUpdateMessage = movementProtocol.createPositionUpdateMessage(player_id_, updated_x, updated_y);

            for (int nearbyPlayerId : nearbyPlayers) {
                if (nearbyPlayerId == player_id_) continue; // Evita enviar para si mesmo

                auto session = sessionManager_.getSession(nearbyPlayerId);
                if (session) {
                    session->sendDataToClient(positionUpdateMessage);
                }
            }

            spdlog::info("Player {} moved to x = {}, y = {} and update sent to nearby players", player_id_, new_x, new_y);
        } else {
            spdlog::warn("Player {} attempted to move to invalid position ({}, {})", player_id_, new_x, new_y);
            // Opcional: Enviar mensagem ao cliente informando que o movimento não é permitido
            sendDataToClient("MovementBlocked");
        }

        receiveClientData();
    }
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
            auto pongMessage = std::make_shared<std::string>("Pong");
            auto self = shared_from_this();
            async_write(socket_, boost::asio::buffer(*pongMessage),
                [this, self, pongMessage](boost::system::error_code ec, std::size_t /*length*/) {
                    if (!ec) {
                        receiveClientData();
                    } else {
                        spdlog::error("Error sending pong message: {}", ec.message());
                    }
                }
            );
            break;
        }
        case ProtocolCommand::MOVE_CHARACTER: {
            handleMovementCommands(message);
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

        auto successMessage = std::make_shared<std::string>("Login successful");
        auto self = shared_from_this();
        boost::asio::async_write(socket_, boost::asio::buffer(*successMessage),
            [this, self, successMessage](boost::system::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    spdlog::info("Login success message sent to client.");
                    receiveClientData();
                } else {
                    spdlog::error("Error sending login success message: {}", ec.message());
                }
            }
        );
    } else {
        spdlog::error("Authentication failed for player: {}", credentials.username);
        socket_.close();
    }
}

void Session::sendDataToClient(const std::string& message) {
    auto self = shared_from_this();
    auto msg = std::make_shared<std::string>(message);
    boost::asio::async_write(socket_, boost::asio::buffer(*msg),
        [this, self, msg](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                // Nada a fazer aqui
            } else {
                spdlog::error("Error on write: {}", ec.message());
            }
        });
}

void Session::sendDataToClient(const std::vector<uint8_t>& message) {
    auto self = shared_from_this();
    auto msg = std::make_shared<std::vector<uint8_t>>(message);
    boost::asio::async_write(socket_, boost::asio::buffer(*msg),
        [this, self, msg](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                // Nada a fazer aqui
            } else {
                spdlog::error("Error on write: {}", ec.message());
            }
        });
}