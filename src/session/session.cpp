// src/Session.cpp
#include "session.h"
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
                std::string message(data_, length);
                spdlog::info("Received from client: {}", message);

                // Here you can process the message and possibly send a response
                // For example, echo the message back to the client
                do_write(length);
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
