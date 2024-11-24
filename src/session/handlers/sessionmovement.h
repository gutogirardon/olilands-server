#ifndef SESSIONMOVEMENT_H
#define SESSIONMOVEMENT_H

#include <vector>
#include "protocol/movementprotocol.h"

class Session;
class DatabaseManager;
class World;
class SessionManager;

class SessionMovement {
public:
    SessionMovement(Session& session, DatabaseManager& dbManager, World& world, SessionManager& sessionManager);
    void handleMovementCommands(const std::vector<uint8_t>& message);

private:
    Session& session_;
    DatabaseManager& dbManager_;
    World& world_;
    SessionManager& sessionManager_;
    MovementProtocol movementProtocol_;
};

#endif // SESSIONMOVEMENT_H
