#include "server.h"
#include "session/session.h"
#include <spdlog/spdlog.h>

Server::Server(boost::asio::io_context& io_context, short port, DatabaseManager& dbManager)
    : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
      dbManager_(dbManager) {  // Inicializa o DatabaseManager
    do_accept();
    spdlog::info("Server listening on port {}", port);
}

void Server::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
            if (!ec) {
                // Certifique-se de que a nova sessão é gerida corretamente por shared_ptr
                auto new_session = std::make_shared<Session>(std::move(socket), dbManager_);
                new_session->beginSession();
            }
            else {
                spdlog::error("Error accepting connection: {}", ec.message());
            }

            // Continua aceitando novas conexões
            do_accept();
        });
}

