#include "chunker.hpp"
#include "logging.hpp"
#include <sstream>
#include <algorithm>

namespace Chunker {

ChunkConfig defaultConfig() {
    ChunkConfig cfg;
    cfg.strategy = ChunkStrategy::SENTENCE;
    cfg.maxChunkSize = 512;
    cfg.overlap = 64;
    cfg.delimiter = "\n";
    return cfg;
}

std::vector<TextChunk> chunk(const std::string& text, const ChunkConfig& config) {
    switch (config.strategy) {
        case ChunkStrategy::FIXED_SIZE:
            return fixedSizeChunk(text, config.maxChunkSize, config.overlap);
        case ChunkStrategy::SENTENCE:
            return sentenceChunk(text, config.maxChunkSize);
        case ChunkStrategy::PARAGRAPH:
            return paragraphChunk(text, config.maxChunkSize);
        case ChunkStrategy::RECURSIVE:
            return recursiveChunk(text, config.maxChunkSize, config.overlap);
        default:
            return fixedSizeChunk(text, config.maxChunkSize, config.overlap);
    }
}

std::vector<TextChunk> fixedSizeChunk(const std::string& text, int size, int overlap) {
    std::vector<TextChunk> chunks;
    int idx = 0;
    size_t pos = 0;
    while (pos < text.size()) {
        size_t end = std::min(pos + static_cast<size_t>(size), text.size());
        TextChunk tc;
        tc.content = text.substr(pos, end - pos);
        tc.startOffset = static_cast<int>(pos);
        tc.endOffset = static_cast<int>(end);
        tc.index = idx++;
        chunks.push_back(tc);
        pos = end - std::min(static_cast<size_t>(overlap), end);
        if (pos >= text.size()) break;
    }
    return chunks;
}

std::vector<TextChunk> sentenceChunk(const std::string& text, int maxSize) {
    std::vector<TextChunk> chunks;
    std::string current;
    int idx = 0, startOff = 0;
    for (size_t i = 0; i < text.size(); i++) {
        current += text[i];
        bool isSentenceEnd = (text[i] == '.' || text[i] == '!' || text[i] == '?');
        if (isSentenceEnd || static_cast<int>(current.size()) >= maxSize) {
            TextChunk tc;
            tc.content = current;
            tc.startOffset = startOff;
            tc.endOffset = static_cast<int>(i + 1);
            tc.index = idx++;
            chunks.push_back(tc);
            startOff = static_cast<int>(i + 1);
            current.clear();
        }
    }
    if (!current.empty()) {
        TextChunk tc;
        tc.content = current;
        tc.startOffset = startOff;
        tc.endOffset = static_cast<int>(text.size());
        tc.index = idx;
        chunks.push_back(tc);
    }
    return chunks;
}

std::vector<TextChunk> paragraphChunk(const std::string& text, int maxSize) {
    std::vector<TextChunk> chunks;
    std::istringstream stream(text);
    std::string line, current;
    int idx = 0, startOff = 0, pos = 0;
    while (std::getline(stream, line)) {
        if (line.empty() && !current.empty()) {
            TextChunk tc;
            tc.content = current;
            tc.startOffset = startOff;
            tc.endOffset = pos;
            tc.index = idx++;
            chunks.push_back(tc);
            startOff = pos + 1;
            current.clear();
        } else {
            if (!current.empty()) current += "\n";
            current += line;
        }
        pos += static_cast<int>(line.size()) + 1;
    }
    if (!current.empty()) {
        TextChunk tc;
        tc.content = current;
        tc.startOffset = startOff;
        tc.endOffset = static_cast<int>(text.size());
        tc.index = idx;
        chunks.push_back(tc);
    }
    return chunks;
}

std::vector<TextChunk> recursiveChunk(const std::string& text, int maxSize, int overlap) {
    if (static_cast<int>(text.size()) <= maxSize) {
        TextChunk tc;
        tc.content = text;
        tc.startOffset = 0;
        tc.endOffset = static_cast<int>(text.size());
        tc.index = 0;
        return {tc};
    }
    // Try paragraph split first, then sentence, then fixed
    auto result = paragraphChunk(text, maxSize);
    bool needsSplit = false;
    for (auto& chunk : result) {
        if (static_cast<int>(chunk.content.size()) > maxSize) {
            needsSplit = true;
            break;
        }
    }
    if (needsSplit) {
        return fixedSizeChunk(text, maxSize, overlap);
    }
    return result;
}

} // namespace Chunker
