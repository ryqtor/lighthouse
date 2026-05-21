#ifndef VECTOR_INDEX_HPP
#define VECTOR_INDEX_HPP

#include "embedding.hpp"
#include <string>
#include <vector>
#include <map>

struct IndexEntry {
    int id;
    std::string documentId;
    std::string text;
    EmbeddingVector embedding;
};

class VectorIndex {
public:
    VectorIndex();

    int addEntry(const std::string& docId, const std::string& text, const EmbeddingVector& emb);
    bool removeEntry(int entryId);
    std::vector<IndexEntry> search(const EmbeddingVector& query, int topK = 5, float threshold = 0.0f);

    bool saveIndex(const std::string& path);
    bool loadIndex(const std::string& path);

    size_t size() const;
    void clear();

private:
    std::vector<IndexEntry> entries;
    int nextId;
};

#endif // VECTOR_INDEX_HPP
