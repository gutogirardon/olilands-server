#include "session.h"
#include "protocol/loginprotocol.h"
#include <vector>
#include <spdlog/spdlog.h>
#include <exception>

Session::Session(boost::asio::ip::tcp::socket socket)
    : socket_(std::move(socket)) {}

void Session::beginSession() {
    try {
        spdlog::info("Client connected: {}:{}",
                     socket_.remote_endpoint().address().to_string(),
                     socket_.remote_endpoint().port());
    }
    catch (const std::exception& e) {
        spdlog::error("Error obtaining client endpoint: {}", e.what());
    }
    receiveClientData();
}

void Session::receiveClientData() {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                std::vector<uint8_t> message(data_, data_ + length);

                if (player_session_state_ == State::Unauthenticated) {
                    authenticatePlayer(message);
                } else {
                    // Instância de LoginProtocol
                    LoginProtocol login_protocol;
                    PlayerLoginInfo credentials = login_protocol.handleLoginRequest(message);
                    receiveClientData(); // Continue reading after processing
                }
            }
            else {
                if (ec == boost::asio::error::eof) {
                    try {
                        spdlog::info("Client disconnected: {}:{}",
                                     socket_.remote_endpoint().address().to_string(),
                                     socket_.remote_endpoint().port());
                    }
                    catch (const std::exception& e) {
                        spdlog::error("Error obtaining client endpoint: {}", e.what());
                    }
                }
                else {
                    spdlog::error("Error on read: {}", ec.message());
                }
            }
        });
}

void Session::authenticatePlayer(const std::vector<uint8_t>& message) {
    // Instancia o LoginProtocol
    LoginProtocol login_protocol;

    // Usa o LoginProtocol para processar a mensagem de autenticação
    PlayerLoginInfo credentials = login_protocol.handleLoginRequest(message);

    // Verificar se as credenciais são válidas
    if (credentials.username == "testUser" && credentials.password == "testUser") {
        spdlog::info("Player authenticated: {}", credentials.username);
        player_session_state_ = State::Authenticated;
        receiveClientData();  // Continua a leitura após autenticação
    } else {
        spdlog::error("Authentication failed for player: {}", credentials.username);
        socket_.close();  // Fecha a conexão em caso de falha
    }
}

void Session::sendDataToClient(std::size_t length) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                receiveClientData();
            }
            else {
                spdlog::error("Error on write: {}", ec.message());
            }
        });
}
