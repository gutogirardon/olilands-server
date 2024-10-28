#include "sessionmanager.h"
#include "session.h"

void SessionManager::addSession(int playerId, std::shared_ptr<Session> session) {
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_[playerId] = session;
}

void SessionManager::removeSession(int playerId) {
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.erase(playerId);
}

std::shared_ptr<Session> SessionManager::getSession(int playerId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = sessions_.find(playerId);
    if (it != sessions_.end()) {
        return it->second.lock();
    }
    return nullptr;
}
