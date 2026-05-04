#ifndef SSE_HPP
#define SSE_HPP

#include <string>
#include <functional>
#include <cstdint>

struct SSEEvent {
    std::string id;
    std::string event;
    std::string data;
    int retry;
};

typedef std::function<bool(const SSEEvent&)> SSECallback;

class SSEClient {
public:
    SSEClient();
    ~SSEClient();

    bool connect(const char* host, int16_t port, const char* endpoint);
    bool startListening(SSECallback callback);
    void stopListening();
    bool isConnected() const;

    void setRetryInterval(int ms);
    void setLastEventId(const std::string& id);

private:
    bool connected;
    bool listening;
    int retryMs;
    std::string lastEventId;

    SSEEvent parseEvent(const std::string& raw);
    bool reconnect();
};

#endif // SSE_HPP
