#include <iostream>

#include <net/client.h>

class Client : public net::Client
{
public:
    Client(asio::io_service & service, asio::ip::tcp::endpoint & endpoint)
        : net::Client(service, endpoint)
    {
        count_ = 0;
    }

    void onStateChanged(int state) override
    {
        if(state == (int)net::ClientState::Disconnected)
            std::cout << "Disconnected" << std::endl;
        if(state == (int)net::ClientState::Connecting)
            std::cout << "Connecting" << std::endl;
        if(state == (int)net::ClientState::Connected)
        {
            std::cout << "Connected" << std::endl;
            send("1234567890", 10);
        }
    }

    void onMessage(const char * data, uint32_t size) override
    {
        std::cout << "RECV:" << std::string(data, size) << std::endl;
        std::string str = std::to_string(count_);
        send(str.data(), str.size());
        ++count_;
    }

private:
    int count_;
};


int main()
{
    asio::io_service service;
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string("0.0.0.0"), 40000);
    auto client = std::make_shared<Client>(service, ep);
    client->start();
    service.run();
}
