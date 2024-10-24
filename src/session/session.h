#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include "protocol/loginprotocol.h"
#include "database/databasemanager.h"

class Session : public std::enable_shared_from_this<Session> {
public:
    explicit     Session(boost::asio::ip::tcp::socket socket, DatabaseManager& dbManager);
    void beginSession();

private:
    enum class State { Unauthenticated, Authenticated };
    void receiveClientData();
    void sendDataToClient(std::size_t length);
    void authenticatePlayer(const std::vector<uint8_t>& message);

    boost::asio::ip::tcp::socket socket_;
    DatabaseManager& dbManager_;

    enum { max_length = 1024 };
    char data_[max_length]{};

    State player_session_state_ = State::Unauthenticated;
};

#endif // SESSION_H
