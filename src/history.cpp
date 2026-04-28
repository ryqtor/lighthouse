#include "history.hpp"
#include "logging.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include <numeric>

HistoryManager::HistoryManager(size_t maxEntries) : maxEntries(maxEntries) {}

void HistoryManager::addEntry(const std::string& role, const std::string& content, int tokens) {
    HistoryEntry entry;
    entry.timestamp = static_cast<int64_t>(std::time(nullptr));
    entry.role = role;
    entry.content = content;
    entry.tokenCount = tokens;
    historyEntries.push_back(entry);
    enforceLimit();
}

void HistoryManager::removeOldest(int count) {
    for (int i = 0; i < count && !historyEntries.empty(); i++) {
        historyEntries.pop_front();
    }
}

void HistoryManager::clearAll() {
    historyEntries.clear();
}

bool HistoryManager::exportHistory(const std::string& filePath) const {
    std::ofstream ofs(filePath);
    if (!ofs.is_open()) return false;
    for (auto& entry : historyEntries) {
        ofs << entry.timestamp << "|" << entry.role << "|"
            << entry.tokenCount << "|" << entry.content << "\n";
    }
    ofs.close();
    return true;
}

bool HistoryManager::importHistory(const std::string& filePath) {
    std::ifstream ifs(filePath);
    if (!ifs.is_open()) return false;
    historyEntries.clear();
    std::string line;
    while (std::getline(ifs, line)) {
        std::istringstream iss(line);
        HistoryEntry entry;
        std::string token;
        std::getline(iss, token, '|'); entry.timestamp = std::stoll(token);
        std::getline(iss, entry.role, '|');
        std::getline(iss, token, '|'); entry.tokenCount = std::stoi(token);
        std::getline(iss, entry.content);
        historyEntries.push_back(entry);
    }
    return true;
}

size_t HistoryManager::size() const { return historyEntries.size(); }

int HistoryManager::totalTokens() const {
    return std::accumulate(historyEntries.begin(), historyEntries.end(), 0,
        [](int sum, const HistoryEntry& e) { return sum + e.tokenCount; });
}

const std::deque<HistoryEntry>& HistoryManager::entries() const {
    return historyEntries;
}

void HistoryManager::enforceLimit() {
    while (historyEntries.size() > maxEntries) {
        historyEntries.pop_front();
    }
}
