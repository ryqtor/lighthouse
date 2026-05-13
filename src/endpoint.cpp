#include "endpoint.hpp"
#include "logging.hpp"

EndpointRouter::EndpointRouter() : apiPrefix("/api/v1") {}

void EndpointRouter::get(const std::string& path, RouteHandler handler) {
    routes["GET"][apiPrefix + path] = handler;
    Logging::debug("Route registered: GET %s%s", apiPrefix.c_str(), path.c_str());
}

void EndpointRouter::post(const std::string& path, RouteHandler handler) {
    routes["POST"][apiPrefix + path] = handler;
    Logging::debug("Route registered: POST %s%s", apiPrefix.c_str(), path.c_str());
}

HttpResponse EndpointRouter::dispatch(const HttpRequest& request) {
    HttpResponse response;
    auto methodIt = routes.find(request.method);
    if (methodIt == routes.end()) {
        response.statusCode = 405;
        response.body = "{\"error\":\"Method not allowed\"}";
        return response;
    }
    auto routeIt = methodIt->second.find(request.path);
    if (routeIt == methodIt->second.end()) {
        response.statusCode = 404;
        response.body = "{\"error\":\"Not found\"}";
        Logging::warn("Route not found: %s %s", request.method.c_str(), request.path.c_str());
        return response;
    }
    Logging::info("Dispatching: %s %s", request.method.c_str(), request.path.c_str());
    return routeIt->second(request);
}

void EndpointRouter::setPrefix(const std::string& prefix) {
    apiPrefix = prefix;
}

size_t EndpointRouter::routeCount() const {
    size_t count = 0;
    for (auto& [method, paths] : routes) {
        count += paths.size();
    }
    return count;
}
