#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include "protocol/loginprotocol.h"
#include "protocol/characterprotocol.h"
#include "database/databasemanager.h"

class Session : public std::enable_shared_from_this<Session> {
public:
    explicit     Session(boost::asio::ip::tcp::socket socket, DatabaseManager& dbManager);
    void beginSession();

private:
    enum class State { Unauthenticated, Authenticated, CharacterSelection, InGame };

    void receiveClientData();
    void sendDataToClient(const std::string& message);
    void authenticatePlayer(const std::vector<uint8_t>& message);
    void handlePlayerCommands(const std::vector<uint8_t>& message);
    void handleLoginTimeout();
    void handleCharacterSelectionCommands(const std::vector<uint8_t>& message);

    boost::asio::ip::tcp::socket socket_;
    boost::asio::steady_timer login_timer_;
    DatabaseManager& dbManager_;

    enum { max_length = 1024 };
    char data_[max_length]{};

    State player_session_state_ = State::Unauthenticated;
    int account_id_;
    std::string username_;
};

#endif // SESSION_H
