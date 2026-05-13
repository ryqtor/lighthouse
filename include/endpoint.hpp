#ifndef ENDPOINT_HPP
#define ENDPOINT_HPP

#include <string>
#include <functional>
#include <map>

struct HttpRequest {
    std::string method;
    std::string path;
    std::string body;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> queryParams;
};

struct HttpResponse {
    int statusCode;
    std::string body;
    std::map<std::string, std::string> headers;
};

typedef std::function<HttpResponse(const HttpRequest&)> RouteHandler;

class EndpointRouter {
public:
    EndpointRouter();

    void get(const std::string& path, RouteHandler handler);
    void post(const std::string& path, RouteHandler handler);

    HttpResponse dispatch(const HttpRequest& request);

    void setPrefix(const std::string& prefix);
    size_t routeCount() const;

private:
    std::map<std::string, std::map<std::string, RouteHandler>> routes;
    std::string apiPrefix;
};

#endif // ENDPOINT_HPP
