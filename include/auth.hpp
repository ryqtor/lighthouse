#ifndef AUTH_HPP
#define AUTH_HPP

#include <string>
#include <vector>
#include <functional>

enum class AuthResult {
    ALLOWED,
    DENIED,
    TOKEN_EXPIRED,
    TOKEN_INVALID,
    NO_TOKEN
};

typedef std::function<AuthResult(const std::string& token, const std::string& endpoint)> AuthHandler;

class AuthMiddleware {
public:
    AuthMiddleware();

    void setSecret(const std::string& secret);
    void addPublicEndpoint(const std::string& endpoint);
    void addProtectedEndpoint(const std::string& endpoint);

    AuthResult authenticate(const std::string& token, const std::string& endpoint);
    bool isPublicEndpoint(const std::string& endpoint) const;

    std::string generateToken(const std::string& userId, int expirySeconds = 3600);
    bool revokeToken(const std::string& token);

private:
    std::string secretKey;
    std::vector<std::string> publicEndpoints;
    std::vector<std::string> protectedEndpoints;
    std::vector<std::string> revokedTokens;
};

#endif // AUTH_HPP
