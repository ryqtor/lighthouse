#include "ingest.hpp"
#include "logging.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <ctime>
#include <functional>

namespace Ingest {

Document loadFromFile(const std::string& filePath) {
    Document doc;
    doc.source = filePath;
    doc.timestamp = static_cast<int64_t>(std::time(nullptr));

    std::ifstream ifs(filePath);
    if (!ifs.is_open()) {
        Logging::error("Ingest: failed to open %s", filePath.c_str());
        return doc;
    }
    std::stringstream ss;
    ss << ifs.rdbuf();
    doc.content = ss.str();

    // Determine content type from extension
    std::string ext = std::filesystem::path(filePath).extension().string();
    doc.contentType = ext.empty() ? "text/plain" : ext;

    // Generate document ID from path hash
    std::hash<std::string> hasher;
    doc.id = std::to_string(hasher(filePath));

    Logging::info("Ingested document: %s (%zu bytes)", filePath.c_str(), doc.content.size());
    return doc;
}

Document loadFromText(const std::string& text, const std::string& source) {
    Document doc;
    doc.content = text;
    doc.source = source;
    doc.contentType = "text/plain";
    doc.timestamp = static_cast<int64_t>(std::time(nullptr));
    std::hash<std::string> hasher;
    doc.id = std::to_string(hasher(text));
    return doc;
}

std::vector<Document> loadDirectory(const std::string& dirPath, const std::string& extension) {
    std::vector<Document> docs;
    if (!std::filesystem::exists(dirPath)) {
        Logging::error("Ingest: directory not found: %s", dirPath.c_str());
        return docs;
    }
    for (auto& entry : std::filesystem::recursive_directory_iterator(dirPath)) {
        if (entry.is_regular_file() && entry.path().extension().string() == extension) {
            docs.push_back(loadFromFile(entry.path().string()));
        }
    }
    Logging::info("Ingested %zu documents from %s", docs.size(), dirPath.c_str());
    return docs;
}

std::string extractText(const std::string& filePath) {
    Document doc = loadFromFile(filePath);
    return doc.content;
}

bool validateDocument(const Document& doc) {
    if (doc.id.empty()) return false;
    if (doc.content.empty()) return false;
    if (doc.source.empty()) return false;
    return true;
}

} // namespace Ingest
