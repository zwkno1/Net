
#pragma once

#include <array>
#include <list>
#include <vector>
#include <mutex>

#include <util.h>
#include <net/detail/config.h>
#include <net/messagemaker.h>


namespace net
{
    
class Session : public enable_shared_from_this<Session>
{
    typedef asio::ip::tcp::socket socket_type;
public:
    Session(asio::ip::tcp::socket & sock)
        : socket_(std::move(sock))
        , timeStamp_(0)
        , stopped_(true)
    {
        recvBuf_.resize(65535);
        update();
    }

    virtual ~Session()
    {
        stop();
    }

    void send(const char * data, uint32_t size, uint32_t flag = 0)
    {
        if(stopped_)
            return;
        auto msg = MessageMaker::make(data, size, flag);
        SYNC(lock, mutex_);
        sendBuf_.push_back(std::move(msg));
        if(sendBuf_.size() == 1)
            doSend();
    }

    void start()
    {
        bool b = true;
        if(stopped_.compare_exchange_strong(b, false))
        {
            doRecive();
        }
    }

    void stop()
    {
        bool b = false;
        if(stopped_.compare_exchange_strong(b, true))
        {
            socket_.close();
            onClose();
        }
    }

    inline int64_t timeStamp()
    {
        return timeStamp_;
    }

private:
    void doRecive()
    {
        auto self = shared_from_this();
        asio::async_read(socket_, asio::buffer(recvBuf_), asio::transfer_exactly(MSG_HEAD_SIZE), 
                                [this, self](const error_code & err, size_t size)
        {
            if(!err)
            {
                assert(size == MSG_HEAD_SIZE);
                //msgSize_ = util::endian::letoh(*((uint32_t *)&recvBuf_[0]));
                //msgFlag_ = util::endian::letoh(*((uint32_t *)&recvBuf_[4]));
                msgSize_ = *((uint32_t *)&recvBuf_[0]);
                msgFlag_ = *((uint32_t *)&recvBuf_[4]);
                if(msgSize_ != 0)
                {
                    if(recvBuf_.size() < msgSize_)
                        recvBuf_.resize(msgSize_);

                    asio::async_read(socket_, asio::buffer(recvBuf_), asio::transfer_exactly(msgSize_),
                                     [this, self](const error_code & err, size_t size)
                    {
                        if(!err)
                        {
                            assert(msgSize_ == size);
                            onMessage(&recvBuf_[0], size);
                            doRecive();
                        }
                        else
                        {
                            onError(err);
                        }
                    });
                }
                else
                {
                    //heartbeat
                    doRecive();
                }
                update();
            }
            else
            {
                onError(err);
            }
        });
    }

    void doSend()
    {
        auto self = shared_from_this();
        socket_.async_send(asio::buffer(sendBuf_.front()),
                           [this, self](const error_code & err, size_t size)
        {
            if(!err)
            {
                SYNC(lock, mutex_);
                assert(size == sendBuf_.front().size());

                sendBuf_.pop_front();
                if(!sendBuf_.empty())
                    doSend();
            }
            else
            {
                onError(err);
            }
        });
    }

    void update()
    {
        timeStamp_ = util::steady_time::now().toMilliSeconds();
    }
    
    void onError(const error_code & err)
    {
        (void)err;
        std::cout << err.message() << std::endl;
        stop();
    }

    virtual void onMessage(const char * data, size_t size) = 0;

    virtual void onClose() = 0;

private:

    asio::ip::tcp::socket socket_;

    std::mutex mutex_;

    std::list<std::vector<char> > sendBuf_;

    std::vector<char> recvBuf_;

    uint32_t msgSize_;

    uint32_t msgFlag_;

    std::atomic<std::int64_t> timeStamp_;
    
    std::atomic<bool> stopped_;
};

} // namespace net
