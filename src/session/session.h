#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <vector>
#include "database/databasemanager.h"
#include "world/world.h"

class SessionManager;

class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(boost::asio::ip::tcp::socket socket, DatabaseManager& dbManager, World& world, SessionManager& sessionManager);

    void beginSession();

private:
    enum class State { Unauthenticated, Authenticated, CharacterSelection, InGame };

    void receiveClientData();
    void sendDataToClient(const std::vector<uint8_t>& message);
    void authenticatePlayer(const std::vector<uint8_t>& message);
    void handlePlayerCommands(const std::vector<uint8_t>& message);
    void handleMovementCommands(const std::vector<uint8_t>& message);
    void handleLoginTimeout();
    void handleCharacterSelectionCommands(const std::vector<uint8_t>& message);
    void handleDisconnection(const boost::system::error_code& ec);

    void startPositionUpdateTimer();
    void savePlayerPosition();

    boost::asio::ip::tcp::socket socket_;
    boost::asio::steady_timer login_timer_;
    boost::asio::steady_timer position_update_timer_;

    DatabaseManager& dbManager_;
    World& world_;
    SessionManager& sessionManager_;

    enum { max_length = 1024 };
    char data_[max_length]{};

    State player_session_state_ = State::Unauthenticated;
    int account_id_;
    std::string username_;
    int player_id_;
    bool is_position_valid = false;

    int last_saved_x_ = 0;
    int last_saved_y_ = 0;
    int last_saved_z_ = 0;
};

#endif // SESSION_H
