#ifndef CHUNKER_HPP
#define CHUNKER_HPP

#include <string>
#include <vector>

enum class ChunkStrategy {
    FIXED_SIZE,
    SENTENCE,
    PARAGRAPH,
    RECURSIVE
};

struct ChunkConfig {
    ChunkStrategy strategy;
    int maxChunkSize;
    int overlap;
    std::string delimiter;
};

struct TextChunk {
    std::string content;
    int startOffset;
    int endOffset;
    int index;
};

namespace Chunker {
    std::vector<TextChunk> chunk(const std::string& text, const ChunkConfig& config);
    std::vector<TextChunk> fixedSizeChunk(const std::string& text, int size, int overlap);
    std::vector<TextChunk> sentenceChunk(const std::string& text, int maxSize);
    std::vector<TextChunk> paragraphChunk(const std::string& text, int maxSize);
    std::vector<TextChunk> recursiveChunk(const std::string& text, int maxSize, int overlap);
    ChunkConfig defaultConfig();
}

#endif // CHUNKER_HPP
