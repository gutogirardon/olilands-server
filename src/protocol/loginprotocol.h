#ifndef LOGIN_PROTOCOL_H
#define LOGIN_PROTOCOL_H

#include "protocol.h"
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

struct LoginCredentials {
    std::string username;
    std::string password;
};

class LoginProtocol : public Protocol {
public:
    LoginProtocol() = default;

    LoginCredentials processLoginMessage(const std::vector<uint8_t>& message);
    void processMessage(const std::vector<uint8_t>& message) override;

private:
    std::string extractString(const std::vector<uint8_t>& message, size_t start);
};

#endif // LOGIN_PROTOCOL_H
