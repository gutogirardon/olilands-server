#include "session.h"
#include "protocol/loginprotocol.h"
#include "database/useraccountmanager.h"
#include "database/playermanager.h"
#include "models/characterinfo.h"
#include <vector>
#include <spdlog/spdlog.h>
#include <exception>

Session::Session(boost::asio::ip::tcp::socket socket, DatabaseManager& dbManager)
    : socket_(std::move(socket)),
      dbManager_(dbManager),
      login_timer_(socket_.get_executor())
{}

void Session::beginSession() {
    try {
        spdlog::info("Client connected: {}:{}",
                     socket_.remote_endpoint().address().to_string(),
                     socket_.remote_endpoint().port());

        // Inicia o timer para o timeout do login (e.g., 3 segundos)
        login_timer_.expires_after(std::chrono::seconds(3));

        // Capturar 'self' para garantir que o objeto permaneça vivo durante a operação assíncrona
        auto self = shared_from_this();
        login_timer_.async_wait([self](const boost::system::error_code& ec) {
            if (!ec) {
                self->handleLoginTimeout();  // Timeout ocorreu
            }
        });

        // Começa a receber os dados do cliente
        receiveClientData();
    }
    catch (const std::exception& e) {
        spdlog::error("Error: {}", e.what());
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
                if (ec == boost::asio::error::operation_aborted) {
                    spdlog::debug("Operation canceled as expected.");
                }
                else if (ec == boost::asio::error::eof) {
                    spdlog::info("Client disconnected");
                }
                else {
                    spdlog::error("Error on read: {}", ec.message());
                }
            }
        });
}

void Session::handleCharacterSelectionCommands(const std::vector<uint8_t>& message) {
    CharacterProtocol characterProtocol;
    ProtocolCommand command = characterProtocol.getCommandFromMessage(message);

    switch (command) {
        case ProtocolCommand::REQUEST_CHARACTER_LIST: {
            PlayerManager playerManager(dbManager_);
            std::vector<CharacterInfo> characters = playerManager.getCharactersForAccount(account_id_);

            // Serializar a lista de personagens para enviar ao cliente
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
                player_session_state_ = State::InGame;

                // Aqui você pode carregar os dados do personagem e iniciar a sessão de jogo

                // Enviar confirmação ao cliente
                sendDataToClient("CharacterSelected|" + characterInfo.name);

                // Continuar recebendo comandos do jogador no estado de jogo
                receiveClientData();
            } else {
                spdlog::error("Character {} not found for account {}", selectedCharacter.name, account_id_);
                sendDataToClient("CharacterSelectionFailed");
                receiveClientData();
            }
            break;
        }
        case ProtocolCommand::CREATE_CHARACTER: {
                    // Mover o tratamento de criação de personagem para cá
                    CharacterCreationInfo creationInfo = characterProtocol.handleCharacterCreationRequest(message);

                    PlayerManager playerManager(dbManager_);
                    if (playerManager.createCharacter(creationInfo.name, creationInfo.vocation, account_id_)) {
                        spdlog::info("Character created successfully.");

                        // Enviar uma resposta de sucesso ao cliente
                        sendDataToClient("Character created successfully");
                    } else {
                        spdlog::error("Character creation failed.");

                        // Enviar uma mensagem de erro ao cliente
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

void Session::handlePlayerCommands(const std::vector<uint8_t>& message) {
    LoginProtocol loginProtocol;
    ProtocolCommand command = loginProtocol.getCommandFromMessage(message);

    switch (command) {
    case ProtocolCommand::LOGOUT: {
            spdlog::info("Player logged out.");
            socket_.close();
            break;
    }
    case ProtocolCommand::PING: {
            spdlog::info("Received ping from client.");

            // Enviar resposta de pong ao cliente
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
    default: {
            spdlog::error("Unknown command received: {}", static_cast<int>(command));
            receiveClientData();
            break;
    }
    }
}


void Session::handleLoginTimeout() {
    spdlog::warn("Login timeout for client");

    // Fechar o socket, liberando os recursos
    boost::system::error_code ec;
    socket_.close(ec);  // Fecha o socket sem lançar exceções

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

        // Enviar uma resposta de sucesso ao cliente
        auto successMessage = std::make_shared<std::string>("Login successful");

        auto self = shared_from_this();
        boost::asio::async_write(socket_, boost::asio::buffer(*successMessage),
            [this, self, successMessage](boost::system::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    spdlog::info("Login success message sent to client.");
                    receiveClientData();  // Agora aguardamos comandos de seleção de personagem
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
