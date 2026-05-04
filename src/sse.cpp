#include "sse.hpp"
#include "logging.hpp"
#include <sstream>
#include <thread>
#include <chrono>

SSEClient::SSEClient() : connected(false), listening(false), retryMs(3000) {}

SSEClient::~SSEClient() {
    stopListening();
}

bool SSEClient::connect(const char* host, int16_t port, const char* endpoint) {
    Logging::info("SSE connecting to %s:%d%s", host, port, endpoint);
    connected = true;
    return true;
}

bool SSEClient::startListening(SSECallback callback) {
    if (!connected) {
        Logging::error("SSE: not connected");
        return false;
    }
    listening = true;
    Logging::info("SSE listener started");
    return true;
}

void SSEClient::stopListening() {
    listening = false;
    connected = false;
    Logging::debug("SSE listener stopped");
}

bool SSEClient::isConnected() const {
    return connected;
}

void SSEClient::setRetryInterval(int ms) {
    retryMs = ms;
}

void SSEClient::setLastEventId(const std::string& id) {
    lastEventId = id;
}

SSEEvent SSEClient::parseEvent(const std::string& raw) {
    SSEEvent event;
    event.retry = 0;
    std::istringstream stream(raw);
    std::string line;
    while (std::getline(stream, line)) {
        if (line.substr(0, 5) == "data:") {
            event.data += line.substr(5);
        } else if (line.substr(0, 6) == "event:") {
            event.event = line.substr(6);
        } else if (line.substr(0, 3) == "id:") {
            event.id = line.substr(3);
        } else if (line.substr(0, 6) == "retry:") {
            event.retry = std::stoi(line.substr(6));
        }
    }
    return event;
}

bool SSEClient::reconnect() {
    Logging::warn("SSE reconnecting in %d ms...", retryMs);
    std::this_thread::sleep_for(std::chrono::milliseconds(retryMs));
    return connect("", 0, "");
}
