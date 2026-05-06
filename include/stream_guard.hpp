#ifndef STREAM_GUARD_HPP
#define STREAM_GUARD_HPP

#include <string>
#include <vector>
#include <functional>

struct StreamFilter {
    std::string pattern;
    std::string replacement;
    bool blockOnMatch;
};

class StreamGuard {
public:
    StreamGuard();

    void addFilter(const std::string& pattern, const std::string& replacement = "", bool block = false);
    void removeFilter(const std::string& pattern);
    void clearFilters();

    std::string processChunk(const std::string& chunk);
    bool shouldBlock(const std::string& chunk) const;

    void setMaxChunkSize(size_t maxSize);
    void enableBuffering(bool enable);

    size_t filterCount() const;

private:
    std::vector<StreamFilter> filters;
    size_t maxChunkSize;
    bool bufferingEnabled;
    std::string pendingBuffer;
};

#endif // STREAM_GUARD_HPP
