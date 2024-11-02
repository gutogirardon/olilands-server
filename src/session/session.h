#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <vector>
#include "database/databasemanager.h"
#include "world/world.h"
#include "models/otherplayerinfo.h"

class SessionManager;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::ip::tcp::socket socket, DatabaseManager& dbManager, World& world, SessionManager& sessionManager);
    void beginSession();

    // Novo método getter
    int getPlayerId() const { return player_id_; }

    void sendDataToClient(const std::vector<uint8_t>& message);

private:
    // Métodos existentes...
    void handleMovementCommands(const std::vector<uint8_t>& message);
    void handlePlayerCommands(const std::vector<uint8_t>& message);
    void handleCharacterSelectionCommands(const std::vector<uint8_t>& message);
    void authenticatePlayer(const std::vector<uint8_t>& message);
    void handleLoginTimeout();
    void receiveClientData();
    void handleDisconnection(const boost::system::error_code& ec);
    void startPositionUpdateTimer();
    void savePlayerPosition();

    // **Novos Métodos**
    void sendNearbyPlayersInfo();
    void broadcastNewPlayerInfo();
    std::vector<uint8_t> createOtherPlayerInfoMessage(const OtherPlayerInfo& info);
    std::string getPlayerName(int playerId) const;
    uint8_t getPlayerState(int playerId) const;

    // **Atributos Existentes**
    boost::asio::ip::tcp::socket socket_;
    DatabaseManager& dbManager_;
    World& world_;
    SessionManager& sessionManager_;
    boost::asio::steady_timer login_timer_;
    boost::asio::steady_timer position_update_timer_;
    enum class State { Unauthenticated, Authenticated, InGame } player_session_state_ = State::Unauthenticated;
    int account_id_ = 0;
    std::string username_;
    int player_id_ = 0;
    bool is_position_valid = false;
    float last_saved_x_;
    float last_saved_y_;
    float last_saved_z_;
    static const int max_length = 1024;
    uint8_t data_[max_length];
};

#endif // SESSION_H