#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <vector>
#include "database/databasemanager.h"
#include "world/world.h"
#include "models/otherplayerinfo.h"
#include "handlers/sessionattack.h"
#include "handlers/sessionmovement.h"

class SessionManager;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::ip::tcp::socket socket, DatabaseManager& dbManager, World& world, SessionManager& sessionManager);
    void beginSession();
    int getPlayerId() const { return player_id_; }
    void sendDataToClient(const std::vector<uint8_t>& message);
    void receiveClientData();

private:
    void handlePlayerCommands(const std::vector<uint8_t>& message);
    void handleCharacterSelectionCommands(const std::vector<uint8_t>& message);
    void authenticatePlayer(const std::vector<uint8_t>& message);
    void handleLoginTimeout();
    void handleDisconnection(const boost::system::error_code& ec);
    void startPositionUpdateTimer();
    void savePlayerPosition();
    void sendNearbyPlayersInfo();
    void broadcastNewPlayerInfo();
    std::vector<uint8_t> createOtherPlayerInfoMessage(const OtherPlayerInfo& info);
    std::string getPlayerName(int playerId) const;
    uint8_t getPlayerState(int playerId) const;

    enum class State { Unauthenticated, Authenticated, InGame } player_session_state_ = State::Unauthenticated;
    boost::asio::ip::tcp::socket socket_;
    DatabaseManager& dbManager_;
    World& world_;
    SessionManager& sessionManager_;
    boost::asio::steady_timer login_timer_;
    boost::asio::steady_timer position_update_timer_;
    std::string username_;
    int account_id_ = 0;
    int player_id_ = 0;
    bool is_position_valid = false;
    float last_saved_x_;
    float last_saved_y_;
    float last_saved_z_;
    static const int max_length = 1024;
    uint8_t data_[max_length];

    SessionAttack attackHandler_;
    SessionMovement movementHandler_;
};

#endif // SESSION_H
