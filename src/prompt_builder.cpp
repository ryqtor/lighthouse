#include "prompt_builder.hpp"
#include "logging.hpp"
#include <sstream>
#include <algorithm>
#include <cmath>

PromptBuilder::PromptBuilder() : maxTokens(4096) {}

PromptBuilder& PromptBuilder::setSystemPrompt(const std::string& prompt) {
    systemPrompt = prompt;
    return *this;
}

PromptBuilder& PromptBuilder::addContext(const std::string& label, const std::string& content, int priority) {
    PromptSection section;
    section.label = label;
    section.content = content;
    section.priority = priority;
    contextSections.push_back(section);
    return *this;
}

PromptBuilder& PromptBuilder::setUserQuery(const std::string& query) {
    userQuery = query;
    return *this;
}

PromptBuilder& PromptBuilder::setMaxTokens(int tokens) {
    maxTokens = tokens;
    return *this;
}

PromptBuilder& PromptBuilder::setTemplate(const std::string& tmpl) {
    templateStr = tmpl;
    return *this;
}

std::string PromptBuilder::build() const {
    std::ostringstream oss;

    // System prompt
    if (!systemPrompt.empty()) {
        oss << systemPrompt << "\n\n";
    }

    // Sort context sections by priority (higher first)
    auto sorted = contextSections;
    std::sort(sorted.begin(), sorted.end(),
        [](const PromptSection& a, const PromptSection& b) { return a.priority > b.priority; });

    // Add context sections within token budget
    int budgetChars = maxTokens * 4; // rough estimate
    int used = static_cast<int>(systemPrompt.size() + userQuery.size());

    for (auto& section : sorted) {
        if (used + static_cast<int>(section.content.size()) > budgetChars) {
            Logging::warn("PromptBuilder: skipping section '%s' (budget exceeded)", section.label.c_str());
            continue;
        }
        oss << "### " << section.label << "\n" << section.content << "\n\n";
        used += static_cast<int>(section.content.size() + section.label.size() + 8);
    }

    // User query
    if (!userQuery.empty()) {
        oss << userQuery;
    }

    std::string result = oss.str();
    if (!templateStr.empty()) {
        result = applyTemplate(result);
    }
    return result;
}

int PromptBuilder::estimateTokens() const {
    int total = static_cast<int>(systemPrompt.size() + userQuery.size());
    for (auto& s : contextSections) total += static_cast<int>(s.content.size());
    return static_cast<int>(std::ceil(total / 4.0));
}

void PromptBuilder::reset() {
    systemPrompt.clear();
    userQuery.clear();
    templateStr.clear();
    contextSections.clear();
    maxTokens = 4096;
}

std::string PromptBuilder::applyTemplate(const std::string& content) const {
    std::string result = templateStr;
    size_t pos = result.find("{{content}}");
    if (pos != std::string::npos) {
        result.replace(pos, 11, content);
    }
    pos = result.find("{{system}}");
    if (pos != std::string::npos) {
        result.replace(pos, 10, systemPrompt);
    }
    pos = result.find("{{query}}");
    if (pos != std::string::npos) {
        result.replace(pos, 9, userQuery);
    }
    return result;
}
