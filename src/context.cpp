#include "context.hpp"
#include "logging.hpp"
#include <sstream>
#include <algorithm>
#include <cmath>

namespace ContextManager {

static ContextWindow ctx = {4096, 512, 0};

void setMaxTokens(int maxTokens) {
    ctx.maxTokens = maxTokens;
}

void setReservedTokens(int reserved) {
    ctx.reservedTokens = reserved;
}

int getAvailableTokens() {
    return ctx.maxTokens - ctx.reservedTokens - ctx.currentUsage;
}

bool shouldTrim(int pendingTokens) {
    return (ctx.currentUsage + pendingTokens) > (ctx.maxTokens - ctx.reservedTokens);
}

int estimateTokenCount(const std::string& text) {
    // rough estimate: ~4 chars per token for English
    return static_cast<int>(std::ceil(text.size() / 4.0));
}

std::string trimToFit(const std::string& content, int maxTokens) {
    int estimated = estimateTokenCount(content);
    if (estimated <= maxTokens) return content;

    // truncate from the beginning, keep recent context
    size_t keepChars = static_cast<size_t>(maxTokens * 4);
    if (keepChars >= content.size()) return content;
    size_t startPos = content.size() - keepChars;
    // find next newline to avoid cutting mid-sentence
    size_t nlPos = content.find('\n', startPos);
    if (nlPos != std::string::npos && nlPos < content.size()) {
        startPos = nlPos + 1;
    }
    Logging::debug("Context trimmed: %zu -> %zu chars", content.size(), content.size() - startPos);
    return content.substr(startPos);
}

std::vector<std::string> splitByParagraphs(const std::string& text) {
    std::vector<std::string> paragraphs;
    std::istringstream stream(text);
    std::string line, current;
    while (std::getline(stream, line)) {
        if (line.empty() && !current.empty()) {
            paragraphs.push_back(current);
            current.clear();
        } else {
            if (!current.empty()) current += "\n";
            current += line;
        }
    }
    if (!current.empty()) paragraphs.push_back(current);
    return paragraphs;
}

} // namespace ContextManager
