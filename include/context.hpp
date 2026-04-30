#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <string>
#include <vector>

struct ContextWindow {
    int maxTokens;
    int reservedTokens;
    int currentUsage;
};

namespace ContextManager {
    void setMaxTokens(int maxTokens);
    void setReservedTokens(int reserved);
    int getAvailableTokens();
    bool shouldTrim(int pendingTokens);
    int estimateTokenCount(const std::string& text);
    std::string trimToFit(const std::string& content, int maxTokens);
    std::vector<std::string> splitByParagraphs(const std::string& text);
}

#endif // CONTEXT_HPP
