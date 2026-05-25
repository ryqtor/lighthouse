#ifndef RAG_HPP
#define RAG_HPP

#include "vector_index.hpp"
#include "ingest.hpp"
#include <string>
#include <vector>

struct RAGContext {
    std::string query;
    std::vector<std::string> retrievedChunks;
    std::string assembledPrompt;
    float avgRelevance;
};

class RAGPipeline {
public:
    RAGPipeline();

    bool ingestDocument(const Document& doc);
    bool ingestDirectory(const std::string& dirPath);

    RAGContext retrieve(const std::string& query, int topK = 3);
    std::string assemblePrompt(const RAGContext& context, const std::string& systemPrompt = "");

    void setChunkSize(int size);
    void setOverlap(int overlap);
    void setRelevanceThreshold(float threshold);

    size_t indexSize() const;

private:
    VectorIndex index;
    int chunkSize;
    int chunkOverlap;
    float relevanceThreshold;

    std::vector<std::string> chunkText(const std::string& text);
};

#endif // RAG_HPP
