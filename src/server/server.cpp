#include "server.h"
#include "session/session.h"
#include <spdlog/spdlog.h>

Server::Server(boost::asio::io_context& io_context, short port, DatabaseManager& dbManager, World& world)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
      dbManager_(dbManager),
      world_(world) {  // Inicializa o World
    do_accept();
    spdlog::info("Server listening on port {}", port);
}

void Server::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
            if (!ec) {
                // Passe world_ para a nova sessão
                auto new_session = std::make_shared<Session>(std::move(socket), dbManager_, world_);
                new_session->beginSession();
            }
            else {
                spdlog::error("Error accepting connection: {}", ec.message());
            }

            do_accept();
        });
}

