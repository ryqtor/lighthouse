#include "migration.hpp"
#include "logging.hpp"
#include <fstream>
#include <filesystem>
#include <sstream>

namespace Migration {

static const int LATEST_VERSION = 3;

int getCurrentVersion(const std::string& dbPath) {
    std::string versionFile = dbPath + "/.schema_version";
    if (!std::filesystem::exists(versionFile)) return 0;
    std::ifstream ifs(versionFile);
    int version = 0;
    ifs >> version;
    return version;
}

bool applyMigration(const std::string& dbPath, int targetVersion) {
    Logging::info("Applying migration to version %d", targetVersion);
    std::string versionFile = dbPath + "/.schema_version";
    std::ofstream ofs(versionFile);
    ofs << targetVersion;
    ofs.close();
    return true;
}

bool runMigrations(const std::string& dbPath) {
    int current = getCurrentVersion(dbPath);
    if (current >= LATEST_VERSION) {
        Logging::info("Schema is up to date (v%d)", current);
        return true;
    }
    Logging::info("Migrating schema from v%d to v%d", current, LATEST_VERSION);
    for (int v = current + 1; v <= LATEST_VERSION; v++) {
        if (!applyMigration(dbPath, v)) {
            Logging::error("Migration to v%d failed!", v);
            return false;
        }
    }
    Logging::success("All migrations applied successfully");
    return true;
}

std::vector<SchemaVersion> getMigrationHistory(const std::string& dbPath) {
    std::vector<SchemaVersion> history;
    // Read migration log entries
    std::string logFile = dbPath + "/.migration_log";
    if (!std::filesystem::exists(logFile)) return history;
    std::ifstream ifs(logFile);
    std::string line;
    while (std::getline(ifs, line)) {
        SchemaVersion sv;
        std::istringstream iss(line);
        iss >> sv.version >> sv.timestamp;
        std::getline(iss, sv.description);
        history.push_back(sv);
    }
    return history;
}

} // namespace Migration
