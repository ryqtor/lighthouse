#include "auth.hpp"
#include "jwt.hpp"
#include "logging.hpp"
#include <algorithm>
#include <ctime>

AuthMiddleware::AuthMiddleware() : secretKey("lighthouse_default_secret") {}

void AuthMiddleware::setSecret(const std::string& secret) {
    secretKey = secret;
}

void AuthMiddleware::addPublicEndpoint(const std::string& endpoint) {
    publicEndpoints.push_back(endpoint);
}

void AuthMiddleware::addProtectedEndpoint(const std::string& endpoint) {
    protectedEndpoints.push_back(endpoint);
}

AuthResult AuthMiddleware::authenticate(const std::string& token, const std::string& endpoint) {
    if (isPublicEndpoint(endpoint)) {
        return AuthResult::ALLOWED;
    }
    if (token.empty()) {
        Logging::warn("Auth: no token provided for %s", endpoint.c_str());
        return AuthResult::NO_TOKEN;
    }
    // Check revocation list
    auto it = std::find(revokedTokens.begin(), revokedTokens.end(), token);
    if (it != revokedTokens.end()) {
        Logging::warn("Auth: token has been revoked");
        return AuthResult::TOKEN_INVALID;
    }
    if (!JWT::verify(token, secretKey)) {
        Logging::error("Auth: invalid token for %s", endpoint.c_str());
        return AuthResult::TOKEN_INVALID;
    }
    if (JWT::isExpired(token)) {
        Logging::warn("Auth: token expired for %s", endpoint.c_str());
        return AuthResult::TOKEN_EXPIRED;
    }
    return AuthResult::ALLOWED;
}

bool AuthMiddleware::isPublicEndpoint(const std::string& endpoint) const {
    return std::find(publicEndpoints.begin(), publicEndpoints.end(), endpoint) != publicEndpoints.end();
}

std::string AuthMiddleware::generateToken(const std::string& userId, int expirySeconds) {
    JWTPayload payload;
    payload.sub = userId;
    payload.iss = "lighthouse";
    payload.iat = static_cast<int64_t>(std::time(nullptr));
    payload.exp = payload.iat + expirySeconds;
    return JWT::encode(payload, secretKey);
}

bool AuthMiddleware::revokeToken(const std::string& token) {
    revokedTokens.push_back(token);
    Logging::info("Token revoked successfully");
    return true;
}
