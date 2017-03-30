#include <net/client.h>

#include <net/messagemaker.h>

namespace net
{

Client::Client(asio::io_service& service, const asio::ip::tcp::endpoint& endpoint)
    : service_(service)
    , endpoint_(endpoint)
    , socket_(service)
    , state_((int)ClientState::Disconnected)
{
    recvBuf_.resize(65535);
}

void Client::send(const char *data, uint32_t size, uint32_t flag)
{
    if(state_ != (int)ClientState::Connected)
        return;

    SYNC(lock, mutex_);
    sendBuf_.push_back(MessageMaker::make(data, size, flag));
    if(sendBuf_.size() == 1)
        doSend();
}

void Client::start()
{
    assert(state_ != (int)ClientState::Connected);

    if(state_ == (int)ClientState::Connected)
        return;
    changeState(ClientState::Connecting);

    socket_.async_connect(endpoint_, [this](const error_code& err)
    {
        if(!err)
        {
            changeState(ClientState::Connected);
            doRecive();
        }
        else
        {
            changeState(ClientState::Disconnected);
        }
    });
}

void Client::changeState(ClientState state)
{
    if(state_ == (int)state)
        return;
    state_ = (int)state;
    onStateChanged((int)state_);
}

void Client::onError(const std::error_code& ec)
{
    (void)ec;
    changeState(ClientState::Disconnected);
    SYNC(lock, mutex_);
    sendBuf_.clear();
}

void Client::updateTimestamp()
{
    timestamp_ = util::steady_time::now().toMilliSeconds();
}

void Client::doSend()
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

void Client::doRecive()
{
    auto self = shared_from_this();
    asio::async_read(socket_, asio::buffer(recvBuf_), asio::transfer_exactly(MSG_HEAD_SIZE),
                     [this, self](const error_code & err, size_t size)
    {
        if(!err)
        {
            assert(size == MSG_HEAD_SIZE);
            msgSize_ = *((uint32_t *)&recvBuf_[0]);
            msgFlag_ = *((uint32_t *)&recvBuf_[4]);
            //msgSize_ = util::endian::letoh(*((uint32_t *)&recvBuf_[0]));
            //msgFlag_ = util::endian::letoh(*((uint32_t *)&recvBuf_[4]));
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

            updateTimestamp();
        }
        else
        {
            onError(err);
        }
    });
}

} // namespace net
