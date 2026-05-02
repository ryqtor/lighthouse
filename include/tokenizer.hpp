#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

struct TokenInfo {
    int32_t id;
    std::string text;
    float logprob;
};

namespace Tokenizer {
    int countTokens(const std::string& text);
    std::vector<TokenInfo> tokenize(const std::string& text);
    std::string detokenize(const std::vector<int32_t>& tokenIds);
    bool loadVocabulary(const std::string& vocabPath);
    int getVocabSize();
    void clearCache();
}

#endif // TOKENIZER_HPP
