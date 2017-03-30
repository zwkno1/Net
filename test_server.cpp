
#include <iostream>
#include <thread>

#include <net/server.h>
#include <net/session.h>


using asio::ip::tcp;

class MySession : public net::Session
{
public:
    MySession(tcp::socket & sock)
        : Session(sock)
    {
        std::cout << "session create" << std::endl;
    }

    void onMessage(const char * msg, size_t size) override
    {
        std::cout << "session recive: " << std::string(msg, size) << std::endl;
        send(msg, size);
    }
    
    void onClose() override
    {
        std::cout << "session close" << std::endl;
    }

    ~MySession() override
    {
        std::cout << "session destroy" << std::endl;
    }
};

struct MySessionFactory
{
    typedef std::shared_ptr<MySession> session_type;

    session_type create(tcp::socket & socket, tcp::endpoint & endpoint)
    {
        (void)endpoint;
        return std::make_shared<MySession>(socket);
    }
};

typedef net::server<MySessionFactory>  MyServer;

int main(int argc, char *argv[])
{
    asio::io_service service;
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string("0.0.0.0"), 40000);
    asio::ip::tcp::endpoint ep2(asio::ip::address::from_string("0.0.0.0"), 50000);

    MySessionFactory factory;
    MyServer server(service, ep, factory, 5000);
    MyServer server2(service, ep2, factory, 5000);

    std::thread threads[1];
    for(auto & t : threads)
    {
        t = std::thread([&]()
        {
            service.run();
        });
    }

    for(auto & t : threads)
    {
        t.join();
    }

    return 0;
}
