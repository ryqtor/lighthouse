#ifndef HISTORY_HPP
#define HISTORY_HPP

#include <string>
#include <vector>
#include <deque>
#include <cstdint>

struct HistoryEntry {
    int64_t timestamp;
    std::string role;
    std::string content;
    int tokenCount;
};

class HistoryManager {
public:
    HistoryManager(size_t maxEntries = 1000);

    void addEntry(const std::string& role, const std::string& content, int tokens = 0);
    void removeOldest(int count = 1);
    void clearAll();
    bool exportHistory(const std::string& filePath) const;
    bool importHistory(const std::string& filePath);

    size_t size() const;
    int totalTokens() const;
    const std::deque<HistoryEntry>& entries() const;

private:
    std::deque<HistoryEntry> historyEntries;
    size_t maxEntries;
    void enforceLimit();
};

#endif // HISTORY_HPP
