#include "session.h"
#include "protocol/loginprotocol.h"
#include <vector>
#include <spdlog/spdlog.h>
#include <exception>

Session::Session(boost::asio::ip::tcp::socket socket)
    : socket_(std::move(socket)) {}

void Session::start() {
    try {
        spdlog::info("Client connected: {}:{}",
                     socket_.remote_endpoint().address().to_string(),
                     socket_.remote_endpoint().port());
    }
    catch (const std::exception& e) {
        spdlog::error("Error obtaining client endpoint: {}", e.what());
    }
    do_read();
}

void Session::do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (!ec) {
                std::vector<uint8_t> message(data_, data_ + length);

                // Certifique-se de que o LoginProtocol está corretamente instanciado
                LoginProtocol loginProtocol;
                loginProtocol.processMessage(message); // Chamando o método do LoginProtocol
                do_read(); // Continua a leitura após processar a mensagem
            }
            else {
                if (ec == boost::asio::error::eof) {
                    try {
                        spdlog::info("Client disconnected: {}:{}",
                                     socket_.remote_endpoint().address().to_string(),
                                     socket_.remote_endpoint().port());
                    }
                    catch (const std::exception& e) {
                        spdlog::error("Error obtaining client endpoint: {}", e.what());
                    }
                }
                else {
                    spdlog::error("Error on read: {}", ec.message());
                }
            }
        });
}

void Session::do_write(std::size_t length) {
    auto self(shared_from_this());
    boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                // Continue reading data from the client
                do_read();
            }
            else {
                spdlog::error("Error on write: {}", ec.message());
            }
        });
}
