#include "stream_guard.hpp"
#include "logging.hpp"
#include <algorithm>
#include <regex>

StreamGuard::StreamGuard() : maxChunkSize(8192), bufferingEnabled(false) {}

void StreamGuard::addFilter(const std::string& pattern, const std::string& replacement, bool block) {
    StreamFilter f;
    f.pattern = pattern;
    f.replacement = replacement;
    f.blockOnMatch = block;
    filters.push_back(f);
    Logging::debug("Stream filter added: '%s'", pattern.c_str());
}

void StreamGuard::removeFilter(const std::string& pattern) {
    filters.erase(
        std::remove_if(filters.begin(), filters.end(),
            [&](const StreamFilter& f) { return f.pattern == pattern; }),
        filters.end());
}

void StreamGuard::clearFilters() {
    filters.clear();
    pendingBuffer.clear();
}

std::string StreamGuard::processChunk(const std::string& chunk) {
    std::string result = chunk;
    for (auto& filter : filters) {
        if (filter.blockOnMatch) continue;
        size_t pos = 0;
        while ((pos = result.find(filter.pattern, pos)) != std::string::npos) {
            result.replace(pos, filter.pattern.length(), filter.replacement);
            pos += filter.replacement.length();
        }
    }
    // enforce max chunk size
    if (result.size() > maxChunkSize) {
        result = result.substr(0, maxChunkSize);
    }
    return result;
}

bool StreamGuard::shouldBlock(const std::string& chunk) const {
    for (auto& filter : filters) {
        if (filter.blockOnMatch && chunk.find(filter.pattern) != std::string::npos) {
            return true;
        }
    }
    return false;
}

void StreamGuard::setMaxChunkSize(size_t maxSize) {
    maxChunkSize = maxSize;
}

void StreamGuard::enableBuffering(bool enable) {
    bufferingEnabled = enable;
    if (!enable) pendingBuffer.clear();
}

size_t StreamGuard::filterCount() const {
    return filters.size();
}
