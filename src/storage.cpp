#include "storage.hpp"
#include "logging.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace Storage {

static std::string db_path_global;

bool initDatabase(const std::string& dbPath) {
    db_path_global = dbPath;
    if (!std::filesystem::exists(dbPath)) {
        std::ofstream ofs(dbPath);
        if (!ofs.is_open()) {
            Logging::error("Failed to create database file: %s", dbPath.c_str());
            return false;
        }
        ofs << "{\"sessions\":{}}" << std::endl;
        ofs.close();
    }
    Logging::success("Database initialized at: %s", dbPath.c_str());
    return true;
}

bool saveSession(const std::string& sessionId, const std::string& data) {
    std::string filepath = db_path_global + "/" + sessionId + ".json";
    std::ofstream ofs(filepath);
    if (!ofs.is_open()) {
        Logging::error("Failed to save session: %s", sessionId.c_str());
        return false;
    }
    ofs << data;
    ofs.close();
    Logging::debug("Session saved: %s", sessionId.c_str());
    return true;
}

std::string loadSession(const std::string& sessionId) {
    std::string filepath = db_path_global + "/" + sessionId + ".json";
    std::ifstream ifs(filepath);
    if (!ifs.is_open()) {
        Logging::warn("Session not found: %s", sessionId.c_str());
        return "";
    }
    std::stringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}

bool clearHistory(const std::string& sessionId) {
    std::string filepath = db_path_global + "/" + sessionId + ".json";
    if (std::filesystem::exists(filepath)) {
        std::filesystem::remove(filepath);
        Logging::info("Session cleared: %s", sessionId.c_str());
        return true;
    }
    return false;
}

} // namespace Storage
