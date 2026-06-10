#ifndef METRICS_HPP
#define METRICS_HPP

#include <string>
#include <map>
#include <cstdint>
#include <chrono>

struct SystemMetrics {
    double tokensPerSecond;
    size_t memoryUsageBytes;
    int64_t uptimeSeconds;
    int totalRequests;
    int activeConnections;
    double avgResponseTimeMs;
};

class MetricsCollector {
public:
    MetricsCollector();

    void recordTokens(int count, double durationMs);
    void recordRequest(double responseTimeMs);
    void recordConnection(bool opened);

    SystemMetrics snapshot() const;
    std::string toJSON() const;
    void reset();

private:
    int64_t totalTokens;
    double totalTokenDurationMs;
    int totalRequests;
    double totalResponseTimeMs;
    int activeConnections;
    std::chrono::steady_clock::time_point startTime;
};

#endif // METRICS_HPP
