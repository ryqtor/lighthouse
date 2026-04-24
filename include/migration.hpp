#ifndef MIGRATION_HPP
#define MIGRATION_HPP

#include <string>
#include <vector>

namespace Migration {
    struct SchemaVersion {
        int version;
        std::string description;
        std::string timestamp;
    };

    bool runMigrations(const std::string& dbPath);
    int getCurrentVersion(const std::string& dbPath);
    bool applyMigration(const std::string& dbPath, int targetVersion);
    std::vector<SchemaVersion> getMigrationHistory(const std::string& dbPath);
}

#endif // MIGRATION_HPP
