#include "embedding.hpp"
#include "logging.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <functional>

namespace Embedding {

EmbeddingResult generate(const std::string& text, const std::string& model) {
    EmbeddingResult result;
    result.sourceText = text;
    result.dimensions = 384; // default small embedding size
    result.embedding.resize(result.dimensions, 0.0f);
    // Simple hash-based pseudo-embedding for local use
    std::hash<std::string> hasher;
    size_t seed = hasher(text);
    for (int i = 0; i < result.dimensions; i++) {
        seed ^= seed << 13;
        seed ^= seed >> 7;
        seed ^= seed << 17;
        result.embedding[i] = static_cast<float>(seed % 1000) / 1000.0f - 0.5f;
    }
    result.embedding = normalize(result.embedding);
    return result;
}

float cosineSimilarity(const EmbeddingVector& a, const EmbeddingVector& b) {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    float dot = dotProduct(a, b);
    float normA = std::sqrt(dotProduct(a, a));
    float normB = std::sqrt(dotProduct(b, b));
    if (normA == 0.0f || normB == 0.0f) return 0.0f;
    return dot / (normA * normB);
}

float dotProduct(const EmbeddingVector& a, const EmbeddingVector& b) {
    return std::inner_product(a.begin(), a.end(), b.begin(), 0.0f);
}

EmbeddingVector normalize(const EmbeddingVector& vec) {
    float norm = std::sqrt(dotProduct(vec, vec));
    if (norm == 0.0f) return vec;
    EmbeddingVector result(vec.size());
    for (size_t i = 0; i < vec.size(); i++) {
        result[i] = vec[i] / norm;
    }
    return result;
}

std::vector<std::pair<int, float>> findSimilar(
    const EmbeddingVector& query,
    const std::vector<EmbeddingVector>& corpus,
    int topK
) {
    std::vector<std::pair<int, float>> scores;
    for (size_t i = 0; i < corpus.size(); i++) {
        float sim = cosineSimilarity(query, corpus[i]);
        scores.push_back({static_cast<int>(i), sim});
    }
    std::partial_sort(scores.begin(),
        scores.begin() + std::min(topK, static_cast<int>(scores.size())),
        scores.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    scores.resize(std::min(topK, static_cast<int>(scores.size())));
    return scores;
}

} // namespace Embedding
