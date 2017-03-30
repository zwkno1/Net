
#pragma once

//#define USE_BOOST

#ifdef USE_BOOST

#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>

namespace asio = boost::asio;

using boost::system::error_code;
using boost::enable_shared_from_this;
using boost::make_shared;

#else

#include <memory>

#include <asio.hpp>
#include <asio/steady_timer.hpp>

using std::error_code;
using std::enable_shared_from_this;
using std::make_shared;

#endif

#define SYNC(l, m) \
    std::unique_lock<std::mutex> l(m); (void)l;
