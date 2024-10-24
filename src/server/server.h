#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include "database/databasemanager.h"  // Certifique-se de incluir o DatabaseManager

class Server {
public:
    Server(boost::asio::io_context& io_context, short port, DatabaseManager& dbManager);

private:
    void do_accept();

    boost::asio::ip::tcp::acceptor acceptor_;
    DatabaseManager& dbManager_;  // Adiciona o DatabaseManager como membro
};

#endif // SERVER_H
