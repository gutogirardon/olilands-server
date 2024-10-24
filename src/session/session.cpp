#include "session.h"
#include "protocol/loginprotocol.h"
#include "database/useraccountmanager.h"
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

        // Inicia o timer para o timeout do login (e.g., 10 segundos)
        login_timer_.expires_after(std::chrono::seconds(3));
        login_timer_.async_wait([self = shared_from_this()](const boost::system::error_code& ec) {
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
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                login_timer_.cancel();

                std::vector<uint8_t> message(data_, data_ + length);

                if (player_session_state_ == State::Unauthenticated) {
                    authenticatePlayer(message);
                } else {
                    receiveClientData(); // Continue reading after processing
                }
            } else {
                // Verificar se o erro é "operation_aborted" (cancelamento da leitura)
                if (ec == boost::asio::error::operation_aborted) {
                    // Cancelamento esperado, não acessamos mais o socket
                    spdlog::debug("Operation canceled as expected.");
                }
                else if (ec == boost::asio::error::eof) {
                    // Cliente desconectou normalmente
                    spdlog::info("Client disconnected");
                }
                else {
                    // Registrar qualquer outro erro inesperado
                    spdlog::error("Error on read: {}", ec.message());
                }
            }
        });
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
    // Instancia o LoginProtocol para processar a mensagem de autenticação
    LoginProtocol login_protocol;
    PlayerLoginInfo credentials = login_protocol.handleLoginRequest(message);

    // Instancia o UserAccountManager para validar as credenciais
    UserAccountManager userAccountManager(dbManager_);
    if (userAccountManager.validateLogin(credentials.username, credentials.password)) {
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
