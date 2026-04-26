#include "session.hpp"
#include "logging.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

SessionManager::SessionManager() : lastSaveTimestamp(0) {}

bool SessionManager::createSession(const std::string& sessionId) {
    for (auto& s : sessions) {
        if (s.sessionId == sessionId) {
            Logging::warn("Session '%s' already exists", sessionId.c_str());
            return false;
        }
    }
    SessionEntry entry;
    entry.sessionId = sessionId;
    entry.messageCount = 0;
    entry.active = true;
    sessions.push_back(entry);
    activeSessionId = sessionId;
    Logging::success("Created session: %s", sessionId.c_str());
    return true;
}

bool SessionManager::resumeSession(const std::string& sessionId) {
    for (auto& s : sessions) {
        if (s.sessionId == sessionId) {
            s.active = true;
            activeSessionId = sessionId;
            Logging::info("Resumed session: %s", sessionId.c_str());
            return true;
        }
    }
    Logging::error("Session '%s' not found", sessionId.c_str());
    return false;
}

bool SessionManager::destroySession(const std::string& sessionId) {
    auto it = std::remove_if(sessions.begin(), sessions.end(),
        [&](const SessionEntry& s) { return s.sessionId == sessionId; });
    if (it != sessions.end()) {
        sessions.erase(it, sessions.end());
        if (activeSessionId == sessionId) activeSessionId.clear();
        Logging::info("Destroyed session: %s", sessionId.c_str());
        return true;
    }
    return false;
}

bool SessionManager::persistToDisk(const std::string& storagePath) {
    std::ofstream ofs(storagePath + "/sessions.dat");
    if (!ofs.is_open()) return false;
    for (auto& s : sessions) {
        ofs << s.sessionId << "|" << s.messageCount << "|" << (s.active ? 1 : 0) << "\n";
    }
    ofs.close();
    Logging::debug("Sessions persisted to disk");
    return true;
}

bool SessionManager::restoreFromDisk(const std::string& storagePath) {
    std::ifstream ifs(storagePath + "/sessions.dat");
    if (!ifs.is_open()) return false;
    sessions.clear();
    std::string line;
    while (std::getline(ifs, line)) {
        std::istringstream iss(line);
        SessionEntry entry;
        std::string token;
        std::getline(iss, entry.sessionId, '|');
        std::getline(iss, token, '|');
        entry.messageCount = std::stoi(token);
        std::getline(iss, token, '|');
        entry.active = (token == "1");
        sessions.push_back(entry);
    }
    Logging::info("Restored %zu sessions from disk", sessions.size());
    return true;
}

std::string SessionManager::getActiveSessionId() const {
    return activeSessionId;
}

std::vector<SessionEntry> SessionManager::listSessions() const {
    return sessions;
}
