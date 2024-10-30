#include "loginprotocol.h"

ProtocolCommand LoginProtocol::getCommandFromMessage(const std::vector<uint8_t>& message) {
    return extractCommandFromMessage(message);  // Chama o método protegido da classe base
}

PlayerLoginInfo LoginProtocol::handleLoginRequest(const std::vector<uint8_t>& message) {
    PlayerLoginInfo loginInfo;

    try {
        ProtocolCommand command = extractCommandFromMessage(message);

        if (command == ProtocolCommand::LOGIN) {
            loginInfo.username = extractDataString(message, 1);
            loginInfo.password = extractDataString(message, 1 + loginInfo.username.size() + 1);

            spdlog::info("Login attempt with username: {} and password: {}", loginInfo.username, loginInfo.password);
        } else {
            spdlog::error("Unexpected command received: {}", static_cast<int>(command));
        }
    } catch (const std::exception& e) {
        spdlog::error("Error processing login request: {}", e.what());
    }

    return loginInfo;
}

void LoginProtocol::handleProtocolCommand(const std::vector<uint8_t>& message) {
    ProtocolCommand command;
    try {
        command = extractCommandFromMessage(message);
    } catch (const std::exception& e) {
        spdlog::error("Failed to extract command: {}", e.what());
        return;
    }

    switch (command) {
        case ProtocolCommand::LOGIN:
            handleLoginRequest(message);
            break;
        default:
            spdlog::error("Unhandled protocol command: {}", static_cast<int>(command));
            break;
    }
}

std::string LoginProtocol::extractDataString(const std::vector<uint8_t>& message, size_t start) {
    size_t length = 0;
    while (start + length < message.size() && message[start + length] != 0) {
        length++;
    }
    return {reinterpret_cast<const char*>(&message[start]), length};
}

std::vector<uint8_t> LoginProtocol::createLoginSuccess(uint16_t accountId) {
    LoginSuccessResponse response;
    response.accountId = accountId;

    std::vector<uint8_t> data;
    data.push_back(static_cast<uint8_t>(response.command));

    data.push_back((response.accountId >> 8) & 0xFF);
    data.push_back(response.accountId & 0xFF);

    spdlog::info("Created LoginSuccessResponse: accountId = {}", response.accountId);
    return data;
}

std::vector<uint8_t> LoginProtocol::createLoginFailure(uint8_t errorCode) {
    LoginFailureResponse response;
    response.errorCode = errorCode;

    std::vector<uint8_t> data;
    data.push_back(static_cast<uint8_t>(response.command));
    data.push_back(response.errorCode);

    spdlog::info("Created LoginFailureResponse: errorCode = {}", static_cast<int>(response.errorCode));
    return data;
}
