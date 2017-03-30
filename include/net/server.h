
#pragma once

#include <net/detail/config.h>

#include <net/detail/reaper.h>

namespace net
{

template<typename SessionFactory>
class server
{
public:
    server(asio::io_service & service, const asio::ip::tcp::endpoint & endpoint, SessionFactory & factory, int64_t millionSeconds)
        : service_(service)
        , acceptor_(service_, endpoint)
        , socket_(service_)
        , factory_(factory)
        , reaper_(service, millionSeconds)
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(socket_, peer_, [this](const error_code & err)
        {
            if(!err)
            {
                auto s = factory_.create(socket_, peer_);
                reaper_.add(s);
                s->start();
            }
            do_accept();
        });
    }

private:
    typedef detail::reaper<typename SessionFactory::session_type>  reaper_type;

    asio::io_service & service_;

    asio::ip::tcp::endpoint peer_;

    asio::ip::tcp::acceptor acceptor_;

    asio::ip::tcp::socket socket_;

    SessionFactory & factory_;

    reaper_type reaper_;
};

} // namespace net

