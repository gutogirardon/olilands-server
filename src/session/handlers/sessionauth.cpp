#include "sessionauth.h"
#include "session/session.h"
#include "session/sessionmanager.h"
#include "database/databasemanager.h"
#include "protocol/loginprotocol.h"
#include "database/useraccountmanager.h"
#include <spdlog/spdlog.h>
#include <exception>

SessionAuth::SessionAuth(Session& session, DatabaseManager& dbManager, SessionManager& sessionManager)
    : session_(session), dbManager_(dbManager), sessionManager_(sessionManager), userAccountManager_(dbManager_),
      login_timer_(session_.getExecutor())
{}

void SessionAuth::handleAuthentication(const std::vector<uint8_t>& message) {
    try {
        PlayerLoginInfo credentials = loginProtocol_.handleLoginRequest(message);
        int account_id = 0;

        if (userAccountManager_.validateLogin(credentials.username, credentials.password, account_id)) {
            spdlog::info("Player authenticated: {}", credentials.username);
            session_.setPlayerSessionState(Session::State::Authenticated);
            session_.setAccountId(account_id);
            session_.setUsername(credentials.username);

            // Cria a mensagem de sucesso
            std::vector<uint8_t> successMessage = loginProtocol_.createLoginSuccess(static_cast<uint16_t>(account_id));
            session_.sendDataToClient(successMessage);

            // Cancela o temporizador de login, já que a autenticação foi bem-sucedida
            login_timer_.cancel();

            // Continua recebendo dados do cliente
            session_.receiveClientData();
        } else {
            spdlog::error("Authentication failed for player: {}", credentials.username);

            // Cria a mensagem de falha
            std::vector<uint8_t> failureMessage = loginProtocol_.createLoginFailure(1);
            session_.sendDataToClient(failureMessage);

            // Fecha o socket
            session_.closeSocket();
        }
    } catch (const std::exception& e) {
        spdlog::error("Error during authentication: {}", e.what());
        // Cria a mensagem de falha
        std::vector<uint8_t> failureMessage = loginProtocol_.createLoginFailure(1);
        session_.sendDataToClient(failureMessage);
        session_.closeSocket();
    }
}

void SessionAuth::startLoginTimer() {
    login_timer_.expires_after(std::chrono::seconds(3));
    login_timer_.async_wait([this](const boost::system::error_code& ec) {
        onLoginTimeout(ec);
    });
}

void SessionAuth::onLoginTimeout(const boost::system::error_code& ec) {
    if (!ec) {
        spdlog::warn("Login timeout for client");
        session_.closeSocket();
    }
}
