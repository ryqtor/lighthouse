#ifndef JWT_HPP
#define JWT_HPP

#include <string>
#include <cstdint>
#include <map>

struct JWTPayload {
    std::string sub;
    std::string iss;
    int64_t iat;
    int64_t exp;
    std::map<std::string, std::string> claims;
};

namespace JWT {
    std::string encode(const JWTPayload& payload, const std::string& secret);
    JWTPayload decode(const std::string& token, const std::string& secret);
    bool verify(const std::string& token, const std::string& secret);
    bool isExpired(const std::string& token);
    std::string base64UrlEncode(const std::string& input);
    std::string base64UrlDecode(const std::string& input);
}

#endif // JWT_HPP
