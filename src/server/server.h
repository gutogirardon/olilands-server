#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include "database/databasemanager.h"
#include "session/sessionmanager.h"
#include "world/world.h"

class Server {
public:
    Server(boost::asio::io_context& io_context, short port, DatabaseManager& dbManager, World& world);

private:
    void do_accept();

    boost::asio::ip::tcp::acceptor acceptor_;
    DatabaseManager& dbManager_;
    World& world_;
    SessionManager sessionManager_;
};

#endif // SERVER_H
