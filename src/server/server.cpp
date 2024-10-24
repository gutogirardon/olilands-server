#include "server.h"
#include "session/session.h"
#include <spdlog/spdlog.h>

Server::Server(boost::asio::io_context& io_context, short port)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
    do_accept();
    spdlog::info("Server listening on port {}", port);
}

void Server::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
            if (!ec) {
                // Create a new session for the connected client
                std::make_shared<Session>(std::move(socket))->start();
            }
            else {
                spdlog::error("Error accepting connection: {}", ec.message());
            }

            // Continue accepting new connections
            do_accept();
        });
}
