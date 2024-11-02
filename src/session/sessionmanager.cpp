#include "sessionmanager.h"
#include "session.h"
#include <spdlog/spdlog.h>

void SessionManager::addSession(int playerId, std::shared_ptr<Session> session) {
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_[playerId] = session;
    spdlog::debug("SessionManager: Added session for playerId {}", playerId);
}

void SessionManager::removeSession(int playerId) {
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.erase(playerId);
    spdlog::debug("SessionManager: Removed session for playerId {}", playerId);
}

std::shared_ptr<Session> SessionManager::getSession(int playerId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = sessions_.find(playerId);
    if (it != sessions_.end()) {
        auto session = it->second.lock();
        if (session) {
            spdlog::debug("SessionManager: Found session for playerId {}", playerId);
            return session;
        } else {
            spdlog::debug("SessionManager: Session for playerId {} has expired", playerId);
        }
    } else {
        spdlog::debug("SessionManager: No session found for playerId {}", playerId);
    }
    return nullptr;
}

std::vector<std::shared_ptr<Session>> SessionManager::getAllSessions() {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::shared_ptr<Session>> activeSessions;
    for (auto& [playerId, weakSession] : sessions_) {
        if (auto session = weakSession.lock()) {
            activeSessions.push_back(session);
        }
    }
    return activeSessions;
}