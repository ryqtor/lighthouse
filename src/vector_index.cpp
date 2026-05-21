#include "vector_index.hpp"
#include "logging.hpp"
#include <fstream>
#include <algorithm>

VectorIndex::VectorIndex() : nextId(0) {}

int VectorIndex::addEntry(const std::string& docId, const std::string& text, const EmbeddingVector& emb) {
    IndexEntry entry;
    entry.id = nextId++;
    entry.documentId = docId;
    entry.text = text;
    entry.embedding = emb;
    entries.push_back(entry);
    Logging::debug("Index entry added: id=%d doc=%s", entry.id, docId.c_str());
    return entry.id;
}

bool VectorIndex::removeEntry(int entryId) {
    auto it = std::remove_if(entries.begin(), entries.end(),
        [entryId](const IndexEntry& e) { return e.id == entryId; });
    if (it != entries.end()) {
        entries.erase(it, entries.end());
        return true;
    }
    return false;
}

std::vector<IndexEntry> VectorIndex::search(const EmbeddingVector& query, int topK, float threshold) {
    std::vector<std::pair<float, IndexEntry*>> scored;
    for (auto& entry : entries) {
        float sim = Embedding::cosineSimilarity(query, entry.embedding);
        if (sim >= threshold) {
            scored.push_back({sim, &entry});
        }
    }
    std::partial_sort(scored.begin(),
        scored.begin() + std::min(topK, static_cast<int>(scored.size())),
        scored.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });

    std::vector<IndexEntry> results;
    for (int i = 0; i < std::min(topK, static_cast<int>(scored.size())); i++) {
        results.push_back(*scored[i].second);
    }
    Logging::info("Vector search: %zu results (top %d, threshold %.2f)", results.size(), topK, threshold);
    return results;
}

bool VectorIndex::saveIndex(const std::string& path) {
    std::ofstream ofs(path, std::ios::binary);
    if (!ofs.is_open()) return false;
    size_t count = entries.size();
    ofs.write(reinterpret_cast<const char*>(&count), sizeof(count));
    for (auto& e : entries) {
        size_t len = e.documentId.size();
        ofs.write(reinterpret_cast<const char*>(&e.id), sizeof(e.id));
        ofs.write(reinterpret_cast<const char*>(&len), sizeof(len));
        ofs.write(e.documentId.data(), len);
        len = e.text.size();
        ofs.write(reinterpret_cast<const char*>(&len), sizeof(len));
        ofs.write(e.text.data(), len);
        size_t dims = e.embedding.size();
        ofs.write(reinterpret_cast<const char*>(&dims), sizeof(dims));
        ofs.write(reinterpret_cast<const char*>(e.embedding.data()), dims * sizeof(float));
    }
    Logging::success("Index saved: %zu entries to %s", count, path.c_str());
    return true;
}

bool VectorIndex::loadIndex(const std::string& path) {
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs.is_open()) return false;
    entries.clear();
    size_t count;
    ifs.read(reinterpret_cast<char*>(&count), sizeof(count));
    for (size_t i = 0; i < count; i++) {
        IndexEntry e;
        size_t len;
        ifs.read(reinterpret_cast<char*>(&e.id), sizeof(e.id));
        ifs.read(reinterpret_cast<char*>(&len), sizeof(len));
        e.documentId.resize(len);
        ifs.read(e.documentId.data(), len);
        ifs.read(reinterpret_cast<char*>(&len), sizeof(len));
        e.text.resize(len);
        ifs.read(e.text.data(), len);
        size_t dims;
        ifs.read(reinterpret_cast<char*>(&dims), sizeof(dims));
        e.embedding.resize(dims);
        ifs.read(reinterpret_cast<char*>(e.embedding.data()), dims * sizeof(float));
        entries.push_back(e);
        if (e.id >= nextId) nextId = e.id + 1;
    }
    Logging::success("Index loaded: %zu entries from %s", entries.size(), path.c_str());
    return true;
}

size_t VectorIndex::size() const { return entries.size(); }

void VectorIndex::clear() {
    entries.clear();
    nextId = 0;
}
