#ifndef INGEST_HPP
#define INGEST_HPP

#include <string>
#include <vector>

struct Document {
    std::string id;
    std::string source;
    std::string content;
    std::string contentType;
    int64_t timestamp;
};

namespace Ingest {
    Document loadFromFile(const std::string& filePath);
    Document loadFromText(const std::string& text, const std::string& source = "inline");
    std::vector<Document> loadDirectory(const std::string& dirPath, const std::string& extension = ".txt");
    std::string extractText(const std::string& filePath);
    bool validateDocument(const Document& doc);
}

#endif // INGEST_HPP
