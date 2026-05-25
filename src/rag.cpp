#include "rag.hpp"
#include "embedding.hpp"
#include "logging.hpp"
#include <numeric>
#include <sstream>

RAGPipeline::RAGPipeline() : chunkSize(512), chunkOverlap(64), relevanceThreshold(0.3f) {}

bool RAGPipeline::ingestDocument(const Document& doc) {
    if (!Ingest::validateDocument(doc)) {
        Logging::error("RAG: invalid document");
        return false;
    }
    auto chunks = chunkText(doc.content);
    for (size_t i = 0; i < chunks.size(); i++) {
        auto emb = Embedding::generate(chunks[i]);
        std::string chunkId = doc.id + "_chunk_" + std::to_string(i);
        index.addEntry(chunkId, chunks[i], emb.embedding);
    }
    Logging::info("RAG: ingested document '%s' as %zu chunks", doc.source.c_str(), chunks.size());
    return true;
}

bool RAGPipeline::ingestDirectory(const std::string& dirPath) {
    auto docs = Ingest::loadDirectory(dirPath);
    bool allOk = true;
    for (auto& doc : docs) {
        if (!ingestDocument(doc)) allOk = false;
    }
    return allOk;
}

RAGContext RAGPipeline::retrieve(const std::string& query, int topK) {
    RAGContext ctx;
    ctx.query = query;
    auto queryEmb = Embedding::generate(query);
    auto results = index.search(queryEmb.embedding, topK, relevanceThreshold);
    float totalSim = 0.0f;
    for (auto& entry : results) {
        ctx.retrievedChunks.push_back(entry.text);
        totalSim += Embedding::cosineSimilarity(queryEmb.embedding, entry.embedding);
    }
    ctx.avgRelevance = results.empty() ? 0.0f : totalSim / results.size();
    Logging::info("RAG: retrieved %zu chunks (avg relevance: %.3f)", ctx.retrievedChunks.size(), ctx.avgRelevance);
    return ctx;
}

std::string RAGPipeline::assemblePrompt(const RAGContext& context, const std::string& systemPrompt) {
    std::ostringstream oss;
    if (!systemPrompt.empty()) {
        oss << systemPrompt << "\n\n";
    }
    if (!context.retrievedChunks.empty()) {
        oss << "--- Retrieved Context ---\n";
        for (size_t i = 0; i < context.retrievedChunks.size(); i++) {
            oss << "[" << (i + 1) << "] " << context.retrievedChunks[i] << "\n\n";
        }
        oss << "--- End Context ---\n\n";
    }
    oss << "Question: " << context.query;
    return oss.str();
}

void RAGPipeline::setChunkSize(int size) { chunkSize = size; }
void RAGPipeline::setOverlap(int overlap) { chunkOverlap = overlap; }
void RAGPipeline::setRelevanceThreshold(float threshold) { relevanceThreshold = threshold; }
size_t RAGPipeline::indexSize() const { return index.size(); }

std::vector<std::string> RAGPipeline::chunkText(const std::string& text) {
    std::vector<std::string> chunks;
    if (text.empty()) return chunks;
    size_t pos = 0;
    while (pos < text.size()) {
        size_t end = std::min(pos + static_cast<size_t>(chunkSize), text.size());
        // Try to break at sentence boundary
        if (end < text.size()) {
            size_t lastPeriod = text.rfind('.', end);
            if (lastPeriod > pos && lastPeriod != std::string::npos) {
                end = lastPeriod + 1;
            }
        }
        chunks.push_back(text.substr(pos, end - pos));
        pos = (end > static_cast<size_t>(chunkOverlap)) ? end - chunkOverlap : end;
        if (pos >= text.size()) break;
    }
    return chunks;
}
