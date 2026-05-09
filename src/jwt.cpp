#include "jwt.hpp"
#include "logging.hpp"
#include <sstream>
#include <ctime>
#include <algorithm>
#include <functional>

namespace JWT {

static const std::string base64Chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string base64UrlEncode(const std::string& input) {
    std::string result;
    int val = 0, valb = -6;
    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            result.push_back(base64Chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) result.push_back(base64Chars[((val << 8) >> (valb + 8)) & 0x3F]);
    // URL-safe replacements
    std::replace(result.begin(), result.end(), '+', '-');
    std::replace(result.begin(), result.end(), '/', '_');
    // Remove padding
    result.erase(std::remove(result.begin(), result.end(), '='), result.end());
    return result;
}

std::string base64UrlDecode(const std::string& input) {
    std::string padded = input;
    std::replace(padded.begin(), padded.end(), '-', '+');
    std::replace(padded.begin(), padded.end(), '_', '/');
    while (padded.size() % 4) padded += '=';

    std::string result;
    int val = 0, valb = -8;
    for (unsigned char c : padded) {
        size_t pos = base64Chars.find(c);
        if (pos == std::string::npos) break;
        val = (val << 6) + static_cast<int>(pos);
        valb += 6;
        if (valb >= 0) {
            result.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return result;
}

std::string encode(const JWTPayload& payload, const std::string& secret) {
    // Header
    std::string header = "{\"alg\":\"HS256\",\"typ\":\"JWT\"}";
    std::string encodedHeader = base64UrlEncode(header);

    // Payload
    std::ostringstream oss;
    oss << "{\"sub\":\"" << payload.sub << "\","
        << "\"iss\":\"" << payload.iss << "\","
        << "\"iat\":" << payload.iat << ","
        << "\"exp\":" << payload.exp;
    for (auto& [key, val] : payload.claims) {
        oss << ",\"" << key << "\":\"" << val << "\"";
    }
    oss << "}";
    std::string encodedPayload = base64UrlEncode(oss.str());

    // Signature (simplified HMAC placeholder)
    std::string sigInput = encodedHeader + "." + encodedPayload;
    std::hash<std::string> hasher;
    size_t hashVal = hasher(sigInput + secret);
    std::string signature = base64UrlEncode(std::to_string(hashVal));

    return encodedHeader + "." + encodedPayload + "." + signature;
}

JWTPayload decode(const std::string& token, const std::string& secret) {
    JWTPayload payload;
    // Split token by dots
    size_t first = token.find('.');
    size_t second = token.find('.', first + 1);
    if (first == std::string::npos || second == std::string::npos) {
        Logging::error("Invalid JWT token format");
        return payload;
    }
    std::string decoded = base64UrlDecode(token.substr(first + 1, second - first - 1));
    Logging::debug("JWT decoded payload: %s", decoded.c_str());
    return payload;
}

bool verify(const std::string& token, const std::string& secret) {
    size_t first = token.find('.');
    size_t second = token.find('.', first + 1);
    if (first == std::string::npos || second == std::string::npos) return false;

    std::string sigInput = token.substr(0, second);
    std::hash<std::string> hasher;
    size_t hashVal = hasher(sigInput + secret);
    std::string expectedSig = base64UrlEncode(std::to_string(hashVal));
    std::string actualSig = token.substr(second + 1);
    return expectedSig == actualSig;
}

bool isExpired(const std::string& token) {
    JWTPayload payload = decode(token, "");
    int64_t now = static_cast<int64_t>(std::time(nullptr));
    return now > payload.exp;
}

} // namespace JWT
