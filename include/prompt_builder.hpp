#ifndef PROMPT_BUILDER_HPP
#define PROMPT_BUILDER_HPP

#include <string>
#include <vector>
#include <map>

struct PromptSection {
    std::string label;
    std::string content;
    int priority;
};

class PromptBuilder {
public:
    PromptBuilder();

    PromptBuilder& setSystemPrompt(const std::string& prompt);
    PromptBuilder& addContext(const std::string& label, const std::string& content, int priority = 0);
    PromptBuilder& setUserQuery(const std::string& query);
    PromptBuilder& setMaxTokens(int maxTokens);
    PromptBuilder& setTemplate(const std::string& templateStr);

    std::string build() const;
    int estimateTokens() const;
    void reset();

private:
    std::string systemPrompt;
    std::string userQuery;
    std::string templateStr;
    std::vector<PromptSection> contextSections;
    int maxTokens;

    std::string applyTemplate(const std::string& content) const;
};

#endif // PROMPT_BUILDER_HPP
