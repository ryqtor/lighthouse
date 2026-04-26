#ifndef SESSION_HPP
#define SESSION_HPP

#include <string>
#include <vector>
#include <cstdint>

struct SessionEntry {
    std::string sessionId;
    std::string createdAt;
    std::string lastAccessed;
    int messageCount;
    bool active;
};

class SessionManager {
public:
    SessionManager();

    bool createSession(const std::string& sessionId);
    bool resumeSession(const std::string& sessionId);
    bool destroySession(const std::string& sessionId);
    bool persistToDisk(const std::string& storagePath);
    bool restoreFromDisk(const std::string& storagePath);

    std::string getActiveSessionId() const;
    std::vector<SessionEntry> listSessions() const;

private:
    std::vector<SessionEntry> sessions;
    std::string activeSessionId;
    int64_t lastSaveTimestamp;
};

#endif // SESSION_HPP
