#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <string>
#include <vector>

namespace Storage {
    bool initDatabase(const std::string& dbPath);
    bool saveSession(const std::string& sessionId, const std::string& data);
    std::string loadSession(const std::string& sessionId);
    bool clearHistory(const std::string& sessionId);
}

#endif // STORAGE_HPP
