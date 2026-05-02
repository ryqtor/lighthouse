#include "tokenizer.hpp"
#include "logging.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <regex>

namespace Tokenizer {

static std::unordered_map<std::string, int32_t> vocab;
static std::unordered_map<int32_t, std::string> reverseVocab;
static bool vocabLoaded = false;

int countTokens(const std::string& text) {
    if (vocabLoaded) {
        return static_cast<int>(tokenize(text).size());
    }
    // fallback: estimate ~4 chars per token
    return static_cast<int>(std::ceil(text.size() / 4.0));
}

std::vector<TokenInfo> tokenize(const std::string& text) {
    std::vector<TokenInfo> tokens;
    // Simple whitespace + punctuation tokenization as fallback
    std::regex wordRegex(R"([\w]+|[^\s\w])");
    auto begin = std::sregex_iterator(text.begin(), text.end(), wordRegex);
    auto end = std::sregex_iterator();
    for (auto it = begin; it != end; ++it) {
        TokenInfo ti;
        ti.text = it->str();
        auto found = vocab.find(ti.text);
        ti.id = (found != vocab.end()) ? found->second : -1;
        ti.logprob = 0.0f;
        tokens.push_back(ti);
    }
    return tokens;
}

std::string detokenize(const std::vector<int32_t>& tokenIds) {
    std::string result;
    for (auto id : tokenIds) {
        auto it = reverseVocab.find(id);
        if (it != reverseVocab.end()) {
            if (!result.empty()) result += " ";
            result += it->second;
        }
    }
    return result;
}

bool loadVocabulary(const std::string& vocabPath) {
    std::ifstream ifs(vocabPath);
    if (!ifs.is_open()) {
        Logging::error("Failed to load vocabulary: %s", vocabPath.c_str());
        return false;
    }
    vocab.clear();
    reverseVocab.clear();
    std::string line;
    int32_t id = 0;
    while (std::getline(ifs, line)) {
        vocab[line] = id;
        reverseVocab[id] = line;
        id++;
    }
    vocabLoaded = true;
    Logging::success("Vocabulary loaded: %d tokens", id);
    return true;
}

int getVocabSize() {
    return static_cast<int>(vocab.size());
}

void clearCache() {
    vocab.clear();
    reverseVocab.clear();
    vocabLoaded = false;
}

} // namespace Tokenizer
