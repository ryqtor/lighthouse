#ifndef EMBEDDING_HPP
#define EMBEDDING_HPP

#include <string>
#include <vector>
#include <cstdint>

typedef std::vector<float> EmbeddingVector;

struct EmbeddingResult {
    std::string sourceText;
    EmbeddingVector embedding;
    int dimensions;
};

namespace Embedding {
    EmbeddingResult generate(const std::string& text, const std::string& model = "default");
    float cosineSimilarity(const EmbeddingVector& a, const EmbeddingVector& b);
    float dotProduct(const EmbeddingVector& a, const EmbeddingVector& b);
    EmbeddingVector normalize(const EmbeddingVector& vec);
    std::vector<std::pair<int, float>> findSimilar(
        const EmbeddingVector& query,
        const std::vector<EmbeddingVector>& corpus,
        int topK = 5
    );
}

#endif // EMBEDDING_HPP
