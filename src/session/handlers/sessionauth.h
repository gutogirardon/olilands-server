#ifndef SESSIONAUTH_H
#define SESSIONAUTH_H

#include <vector>
#include <memory>
#include <boost/asio.hpp>
#include "protocol/loginprotocol.h"
#include "database/useraccountmanager.h"

class Session;
class DatabaseManager;
class SessionManager;

class SessionAuth {
public:
    SessionAuth(Session& session, DatabaseManager& dbManager, SessionManager& sessionManager);
    void handleAuthentication(const std::vector<uint8_t>& message);
    void startLoginTimer();

private:
    void onLoginTimeout(const boost::system::error_code& ec);

    Session& session_;
    DatabaseManager& dbManager_;
    SessionManager& sessionManager_;
    LoginProtocol loginProtocol_;
    UserAccountManager userAccountManager_;
    boost::asio::steady_timer login_timer_;
};

#endif // SESSIONAUTH_H
