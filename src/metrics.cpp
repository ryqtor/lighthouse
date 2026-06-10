#include "metrics.hpp"
#include "logging.hpp"
#include <sstream>
#include <iomanip>

MetricsCollector::MetricsCollector()
    : totalTokens(0), totalTokenDurationMs(0), totalRequests(0),
      totalResponseTimeMs(0), activeConnections(0),
      startTime(std::chrono::steady_clock::now()) {}

void MetricsCollector::recordTokens(int count, double durationMs) {
    totalTokens += count;
    totalTokenDurationMs += durationMs;
}

void MetricsCollector::recordRequest(double responseTimeMs) {
    totalRequests++;
    totalResponseTimeMs += responseTimeMs;
}

void MetricsCollector::recordConnection(bool opened) {
    if (opened) activeConnections++;
    else if (activeConnections > 0) activeConnections--;
}

SystemMetrics MetricsCollector::snapshot() const {
    SystemMetrics m;
    m.tokensPerSecond = (totalTokenDurationMs > 0)
        ? (totalTokens / (totalTokenDurationMs / 1000.0)) : 0.0;
    m.memoryUsageBytes = 0; // platform-specific, placeholder
    auto now = std::chrono::steady_clock::now();
    m.uptimeSeconds = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
    m.totalRequests = totalRequests;
    m.activeConnections = activeConnections;
    m.avgResponseTimeMs = (totalRequests > 0) ? (totalResponseTimeMs / totalRequests) : 0.0;
    return m;
}

std::string MetricsCollector::toJSON() const {
    auto m = snapshot();
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    oss << "{";
    oss << "\"tokens_per_second\":" << m.tokensPerSecond << ",";
    oss << "\"memory_bytes\":" << m.memoryUsageBytes << ",";
    oss << "\"uptime_seconds\":" << m.uptimeSeconds << ",";
    oss << "\"total_requests\":" << m.totalRequests << ",";
    oss << "\"active_connections\":" << m.activeConnections << ",";
    oss << "\"avg_response_ms\":" << m.avgResponseTimeMs;
    oss << "}";
    return oss.str();
}

void MetricsCollector::reset() {
    totalTokens = 0;
    totalTokenDurationMs = 0;
    totalRequests = 0;
    totalResponseTimeMs = 0;
    activeConnections = 0;
    startTime = std::chrono::steady_clock::now();
}
