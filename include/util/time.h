
#pragma once

#include <chrono>
#include <string>
#include <ctime>

#include <cstdint>

namespace util
{

namespace detail
{

template<typename Clock>
struct clock_traits
{
    static inline int64_t time_since_epoch_microseconds()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(Clock::now().time_since_epoch()).count();
    }
};

template<typename Clock>
class time
{
    typedef clock_traits<Clock> clock_traits_type;

public:
    time()
        : usec_(0)
    {
    }

    static time now()
    {
        time t;
        t.usec_ = clock_traits_type::time_since_epoch_microseconds();
        return t;
    }

    time seconds(int64_t t)
    {
        return time(t * int64_t(1000000));
    }

    time milliSeconds(int64_t t)
    {
        return time(t * int64_t(1000));
    }

    time microSeconds(int64_t t)
    {
        return time(t);
    }

    time secondsDouble(double t)
    {
        return time(int64_t(t * 1000000));
    }

    time milliSecondsDouble(double t)
    {
        return time(int64_t(t * 1000));
    }

    time microSecondsDouble(double t)
    {
        return time(int64_t(t));
    }

    // operations

    time operator-() const
    {
        return time(-usec_);
    }

    time operator-(const time& rhs) const
    {
        return time(usec_ - rhs.usec_);
    }

    time operator+(const time& rhs) const
    {
        return time(usec_ + rhs.usec_);
    }

    time& operator+=(const time& rhs)
    {
        usec_ += rhs.usec_;
        return *this;
    }

    time& operator-=(const time& rhs)
    {
        usec_ -= rhs.usec_;
        return *this;
    }

    bool operator<(const time& rhs) const
    {
        return usec_ < rhs.usec_;
    }

    bool operator<=(const time& rhs) const
    {
        return usec_ <= rhs.usec_;
    }

    bool operator>(const time& rhs) const
    {
        return usec_ > rhs.usec_;
    }

    bool operator>=(const time& rhs) const
    {
        return usec_ >= rhs.usec_;
    }

    bool operator==(const time& rhs) const
    {
        return usec_ == rhs.usec_;
    }

    bool operator!=(const time& rhs) const
    {
        return usec_ != rhs.usec_;
    }

    double operator/(const time& rhs) const
    {
        return static_cast<double>(usec_) / static_cast<double>(rhs.usec_);
    }

    time& operator*=(int rhs)
    {
        usec_ *= rhs;
        return *this;
    }

    time operator*(int rhs) const
    {
        time t;
        t.usec_ = usec_ * rhs;
        return t;
    }

    time& operator/=(int rhs)
    {
        usec_ /= rhs;
        return *this;
    }

    time operator/(int rhs) const
    {
        time t;
        t.usec_ = usec_ / rhs;
        return t;
    }

    time& operator*=(int64_t rhs)
    {
        usec_ *= rhs;
        return *this;
    }

    time operator*(int64_t rhs) const
    {
        time t;
        t.usec_ = usec_ * rhs;
        return t;
    }

    time& operator/=(int64_t rhs)
    {
        usec_ /= rhs;
        return *this;
    }

    time operator/(int64_t rhs) const
    {
        time t;
        t.usec_ = usec_ / rhs;
        return t;
    }

    time& operator*=(double rhs)
    {
        usec_ = static_cast<int64_t>(static_cast<double>(usec_)* rhs);
        return *this;
    }

    time operator*(double rhs) const
    {
        time t;
        t.usec_ = static_cast<int64_t>(static_cast<double>(usec_)* rhs);
        return t;
    }

    time& operator/=(double rhs)
    {
        usec_ = static_cast<int64_t>(static_cast<double>(usec_) / rhs);
        return *this;
    }

    time operator/(double rhs) const
    {
        time t;
        t.usec_ = static_cast<int64_t>(static_cast<double>(usec_) / rhs);
        return t;
    }

    // convert

    int64_t toSeconds() const
    {
        return usec_ / 1000000;
    }

    int64_t toMilliSeconds() const
    {
        return usec_ / 1000;
    }

    int64_t toMicroSeconds() const
    {
        return usec_;
    }

    double toSecondsDouble() const
    {
        return usec_ / 1000000.0;
    }

    double toMilliSecondsDouble() const
    {
        return usec_ / 1000.0;
    }

    double toMicroSecondsDouble() const
    {
        return static_cast<double>(usec_);
    }

    std::string toDateTime() const
    {
        return toString("%Y-%m-%d %H:%M:%S");
    }

    std::string toDate() const
    {
        return toString("%Y-%m-%d");
    }

    std::string toTime() const
    {
        return toString("%H:%M:%S");
    }

    std::string toString(const std::string& format) const
    {
        time_t time = static_cast<long>(usec_ / 1000000);

        std::tm* t = std::localtime(&time);

        char buf[32];
        if (strftime(buf, sizeof(buf), format.c_str(), t) == 0)
        {
            return std::string();
        }
        return std::string(buf);
    }

private:
    time(int64_t usec)
        : usec_(usec)
    {
    }

    int64_t usec_;
};

}

typedef detail::time<std::chrono::steady_clock> steady_time;

typedef detail::time<std::chrono::system_clock> system_time;

typedef detail::time<std::chrono::high_resolution_clock> high_resolution_time;

} // namespace net
