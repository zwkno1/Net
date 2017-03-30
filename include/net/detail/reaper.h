
#pragma once

#include <set>
#include <mutex>

#include <net/detail/config.h>
#include <util.h>

namespace net
{

namespace detail
{

template<typename Session>
class reaper
{
public:
    reaper(asio::io_service & service, int64_t millionSeconds)
        : timer_(service)
        , millionSeconds_(millionSeconds)
    {
        doCheck();
    }

    void add(Session & s)
    {
        SYNC(lock, mutex_);
        sessions_.insert(s);
    }

    void del(Session & s)
    {
        SYNC(lock, mutex_);
        sessions_.erase(s);
    }

private:
    void doCheck()
    {
        timer_.expires_from_now(std::chrono::milliseconds(500));
        timer_.async_wait([this](const error_code & err)
        {
            if(!err)
            {
                int64_t now = util::steady_time::now().toMilliSeconds();
                SYNC(lock, mutex_);
                for(auto iter = sessions_.begin(); iter != sessions_.end();)
                {
                    if(now - (*iter)->timeStamp() > millionSeconds_)
                    {
                        (*iter)->stop();
                        iter = sessions_.erase(iter);
                    }
                    else
                    {
                        ++iter;
                    }
                }
            }
            doCheck();
        });
    }

private:
    std::mutex mutex_;

    asio::steady_timer timer_;

    int64_t millionSeconds_;

    std::set<Session> sessions_;

};

}

}
