
#pragma once

#include <mutex>
#include <vector>
#include <list>
#include <atomic>

#include <net/detail/config.h>

namespace net
{

enum class ClientState : int
{
    Connecting = 1,
    Connected,
    Disconnected,
};

class Client : public enable_shared_from_this<Client>
{
public:
    Client(asio::io_service & service, const asio::ip::tcp::endpoint& endpoint);

    void start();

    void send(const char * data, uint32_t size, uint32_t flag = 0);

    virtual void onStateChanged(int state) = 0;

    virtual void onMessage(const char * data, uint32_t size) = 0;

    inline int64_t timestamp() const { return timestamp_; }

private:
    void changeState(ClientState state);

    void doSend();

    void doRecive();

    void onError(const error_code & ec);

    void updateTimestamp();

    std::mutex mutex_;

    asio::io_service & service_;

    asio::ip::tcp::endpoint endpoint_;

    asio::ip::tcp::socket socket_;

    std::atomic<int> state_;

    std::list<std::vector<char> > sendBuf_;

    std::vector<char> recvBuf_;

    uint32_t msgSize_;

    uint32_t msgFlag_;

    int64_t timestamp_;
};

}

#include <net/client.ipp>
