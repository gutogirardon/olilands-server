#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <unordered_map>
#include <memory>
#include <mutex>

// Forward declaration da classe Session
class Session;

class SessionManager {
public:
    // Declaração dos métodos
    void addSession(int playerId, std::shared_ptr<Session> session);
    void removeSession(int playerId);
    std::shared_ptr<Session> getSession(int playerId);

private:
    std::unordered_map<int, std::weak_ptr<Session>> sessions_;
    std::mutex mutex_;
};

#endif // SESSIONMANAGER_H
