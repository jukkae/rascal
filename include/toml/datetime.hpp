//     Copyright Toru Niina 2017.
// Distributed under the MIT License.
#ifndef TOML11_DATETIME
#define TOML11_DATETIME
#include <chrono>
#include <tuple>
#include <array>
#include <ostream>
#include <iomanip>
#include <cstdint>
#include <cstdlib>
#include <ctime>

namespace toml
{

enum class month_t : std::int8_t
{
    Jan =  0,
    Feb =  1,
    Mar =  2,
    Apr =  3,
    May =  4,
    Jun =  5,
    Jul =  6,
    Aug =  7,
    Sep =  8,
    Oct =  9,
    Nov = 10,
    Dec = 11
};

struct local_date
{
    std::int16_t year;   // A.D. (like, 2018)
    std::uint8_t month;  // [0, 11]
    std::uint8_t day;    // [1, 31]

    local_date(int y, month_t m, int d)
        : year (static_cast<std::int16_t>(y)),
          month(static_cast<std::uint8_t>(m)),
          day  (static_cast<std::uint8_t>(d))
    {}

    explicit local_date(const std::tm& t)
        : year (static_cast<std::int16_t>(t.tm_year + 1900)),
          month(static_cast<std::uint8_t>(t.tm_mon)),
          day  (static_cast<std::uint8_t>(t.tm_mday))
    {}

    explicit local_date(const std::chrono::system_clock::time_point& tp)
    {
        const auto t = std::chrono::system_clock::to_time_t(tp);
        const auto tmp = std::localtime(&t); //XXX: not threadsafe!
        assert(tmp); // if std::localtime fails, tmp is nullptr
        const std::tm time = *tmp;
        *this = local_date(time);
    }

    explicit local_date(const std::time_t t)
        : local_date(std::chrono::system_clock::from_time_t(t))
    {}

    operator std::chrono::system_clock::time_point() const
    {
        // std::mktime returns date as local time zone. no conversion needed
        std::tm t;
        t.tm_sec   = 0;
        t.tm_min   = 0;
        t.tm_hour  = 0;
        t.tm_mday  = this->day;
        t.tm_mon   = this->month;
        t.tm_year  = this->year - 1900;
        t.tm_wday  = 0; // the value will be ignored
        t.tm_yday  = 0; // the value will be ignored
        t.tm_isdst = -1;
        return std::chrono::system_clock::from_time_t(std::mktime(&t));
    }

    operator std::time_t() const
    {
        return std::chrono::system_clock::to_time_t(
                std::chrono::system_clock::time_point(*this));
    }

    local_date() = default;
    ~local_date() = default;
    local_date(local_date const&) = default;
    local_date(local_date&&)      = default;
    local_date& operator=(local_date const&) = default;
    local_date& operator=(local_date&&)      = default;
};

inline bool operator==(const local_date& lhs, const local_date& rhs)
{
    return std::make_tuple(lhs.year, lhs.month, lhs.day) ==
           std::make_tuple(rhs.year, rhs.month, rhs.day);
}
inline bool operator!=(const local_date& lhs, const local_date& rhs)
{
    return !(lhs == rhs);
}
inline bool operator< (const local_date& lhs, const local_date& rhs)
{
    return std::make_tuple(lhs.year, lhs.month, lhs.day) <
           std::make_tuple(rhs.year, rhs.month, rhs.day);
}
inline bool operator<=(const local_date& lhs, const local_date& rhs)
{
    return (lhs < rhs) || (lhs == rhs);
}
inline bool operator> (const local_date& lhs, const local_date& rhs)
{
    return !(lhs <= rhs);
}
inline bool operator>=(const local_date& lhs, const local_date& rhs)
{
    return !(lhs < rhs);
}

template<typename charT, typename traits>
std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& os, const local_date& date)
{
    os << std::setfill('0') << std::setw(4) << static_cast<int>(date.year     ) << '-';
    os << std::setfill('0') << std::setw(2) << static_cast<int>(date.month + 1) << '-';
    os << std::setfill('0') << std::setw(2) << static_cast<int>(date.day      );
    return os;
}

struct local_time
{
    std::uint8_t  hour;        // [0, 23]
    std::uint8_t  minute;      // [0, 59]
    std::uint8_t  second;      // [0, 60]
    std::uint16_t millisecond; // [0, 999]
    std::uint16_t microsecond; // [0, 999]
    std::uint16_t nanosecond;  // [0, 999]

    local_time(int h, int m, int s,
               int ms = 0, int us = 0, int ns = 0)
        : hour  (static_cast<std::uint8_t>(h)),
          minute(static_cast<std::uint8_t>(m)),
          second(static_cast<std::uint8_t>(s)),
          millisecond(static_cast<std::uint16_t>(ms)),
          microsecond(static_cast<std::uint16_t>(us)),
          nanosecond (static_cast<std::uint16_t>(ns))
    {}

    explicit local_time(const std::tm& t)
        : hour  (static_cast<std::uint8_t>(t.tm_hour)),
          minute(static_cast<std::uint8_t>(t.tm_min)),
          second(static_cast<std::uint8_t>(t.tm_sec)),
          millisecond(0), microsecond(0), nanosecond(0)
    {}

    template<typename Rep, typename Period>
    explicit local_time(const std::chrono::duration<Rep, Period>& t)
    {
        const auto h = std::chrono::duration_cast<std::chrono::hours>(t);
        this->hour = h.count();
        const auto t2 = t - h;
        const auto m = std::chrono::duration_cast<std::chrono::minutes>(t2);
        this->minute = m.count();
        const auto t3 = t2 - m;
        const auto s = std::chrono::duration_cast<std::chrono::seconds>(t3);
        this->second = s.count();
        const auto t4 = t3 - s;
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t4);
        this->millisecond = ms.count();
        const auto t5 = t4 - ms;
        const auto us = std::chrono::duration_cast<std::chrono::microseconds>(t5);
        this->microsecond = us.count();
        const auto t6 = t5 - us;
        const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t6);
        this->nanosecond = ns.count();
    }

    operator std::chrono::nanoseconds() const
    {
        return std::chrono::nanoseconds (this->nanosecond)  +
               std::chrono::microseconds(this->microsecond) +
               std::chrono::milliseconds(this->millisecond) +
               std::chrono::seconds(this->second) +
               std::chrono::minutes(this->minute) +
               std::chrono::hours(this->hour);
    }

    local_time() = default;
    ~local_time() = default;
    local_time(local_time const&) = default;
    local_time(local_time&&)      = default;
    local_time& operator=(local_time const&) = default;
    local_time& operator=(local_time&&)      = default;
};

inline bool operator==(const local_time& lhs, const local_time& rhs)
{
    return std::make_tuple(lhs.hour, lhs.minute, lhs.second, lhs.millisecond, lhs.microsecond, lhs.nanosecond) ==
           std::make_tuple(rhs.hour, rhs.minute, rhs.second, rhs.millisecond, rhs.microsecond, rhs.nanosecond);
}
inline bool operator!=(const local_time& lhs, const local_time& rhs)
{
    return !(lhs == rhs);
}
inline bool operator< (const local_time& lhs, const local_time& rhs)
{
    return std::make_tuple(lhs.hour, lhs.minute, lhs.second, lhs.millisecond, lhs.microsecond, lhs.nanosecond) <
           std::make_tuple(rhs.hour, rhs.minute, rhs.second, rhs.millisecond, rhs.microsecond, rhs.nanosecond);
}
inline bool operator<=(const local_time& lhs, const local_time& rhs)
{
    return (lhs < rhs) || (lhs == rhs);
}
inline bool operator> (const local_time& lhs, const local_time& rhs)
{
    return !(lhs <= rhs);
}
inline bool operator>=(const local_time& lhs, const local_time& rhs)
{
    return !(lhs < rhs);
}

template<typename charT, typename traits>
std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& os, const local_time& time)
{
    os << std::setfill('0') << std::setw(2) << static_cast<int>(time.hour  ) << ':';
    os << std::setfill('0') << std::setw(2) << static_cast<int>(time.minute) << ':';
    os << std::setfill('0') << std::setw(2) << static_cast<int>(time.second);
    if(time.millisecond != 0 || time.microsecond != 0 || time.nanosecond != 0)
    {
        os << '.';
        os << std::setfill('0') << std::setw(3) << static_cast<int>(time.millisecond);
        if(time.microsecond != 0 || time.nanosecond != 0)
        {
            os << std::setfill('0') << std::setw(3) << static_cast<int>(time.microsecond);
            if(time.nanosecond != 0)
            {
                os << std::setfill('0') << std::setw(3) << static_cast<int>(time.nanosecond);
            }
        }
    }
    return os;
}

struct time_offset
{
    std::int8_t hour;   // [-12, 12]
    std::int8_t minute; // [-59, 59]

    time_offset(int h, int m)
        : hour  (static_cast<std::int8_t>(h)),
          minute(static_cast<std::int8_t>(m))
    {}

    operator std::chrono::minutes() const
    {
        return std::chrono::minutes(this->minute) +
               std::chrono::hours(this->hour);
    }

    time_offset() = default;
    ~time_offset() = default;
    time_offset(time_offset const&) = default;
    time_offset(time_offset&&)      = default;
    time_offset& operator=(time_offset const&) = default;
    time_offset& operator=(time_offset&&)      = default;
};

inline bool operator==(const time_offset& lhs, const time_offset& rhs)
{
    return std::make_tuple(lhs.hour, lhs.minute) ==
           std::make_tuple(rhs.hour, rhs.minute);
}
inline bool operator!=(const time_offset& lhs, const time_offset& rhs)
{
    return !(lhs == rhs);
}
inline bool operator< (const time_offset& lhs, const time_offset& rhs)
{
    return std::make_tuple(lhs.hour, lhs.minute) <
           std::make_tuple(rhs.hour, rhs.minute);
}
inline bool operator<=(const time_offset& lhs, const time_offset& rhs)
{
    return (lhs < rhs) || (lhs == rhs);
}
inline bool operator> (const time_offset& lhs, const time_offset& rhs)
{
    return !(lhs <= rhs);
}
inline bool operator>=(const time_offset& lhs, const time_offset& rhs)
{
    return !(lhs < rhs);
}

template<typename charT, typename traits>
std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& os, const time_offset& offset)
{
    if(offset.hour == 0 && offset.minute == 0)
    {
        os << 'Z';
        return os;
    }
    if(static_cast<int>(offset.hour) * static_cast<int>(offset.minute) < 0)
    {
        const int min = static_cast<int>(offset.hour) * 60 + offset.minute;
        if(min < 0){os << '-';} else {os << '+';}
        os << std::setfill('0') << std::setw(2) << min / 60 << ':';
        os << std::setfill('0') << std::setw(2) << min % 60;
        return os;
    }
    if(offset.hour < 0){os << '-';} else {os << '+';}
    os << std::setfill('0') << std::setw(2) << static_cast<int>(offset.hour) << ':';
    os << std::setfill('0') << std::setw(2) << static_cast<int>(offset.minute);
    return os;
}

struct local_datetime
{
    local_date date;
    local_time time;

    local_datetime(local_date d, local_time t): date(d), time(t) {}

    explicit local_datetime(const std::tm& t): date(t), time(t){}

    explicit local_datetime(const std::chrono::system_clock::time_point& tp)
    {
        const auto t = std::chrono::system_clock::to_time_t(tp);
        const auto tmp = std::localtime(&t); //XXX: not threadsafe!
        assert(tmp); // if std::localtime fails, tmp is nullptr

        std::tm time = *tmp;
        this->date = local_date(time);
        this->time = local_time(time);

        // std::tm lacks subsecond information, so diff between tp and tm
        // can be used to get millisecond & microsecond information.
        const auto t_diff = tp -
            std::chrono::system_clock::from_time_t(std::mktime(&time));
        this->time.millisecond = std::chrono::duration_cast<
            std::chrono::milliseconds>(t_diff).count();
        this->time.microsecond = std::chrono::duration_cast<
            std::chrono::microseconds>(t_diff).count();
        this->time.nanosecond = std::chrono::duration_cast<
            std::chrono::nanoseconds >(t_diff).count();
    }

    explicit local_datetime(const std::time_t t)
        : local_datetime(std::chrono::system_clock::from_time_t(t))
    {}

    operator std::chrono::system_clock::time_point() const
    {
        using internal_duration =
            typename std::chrono::system_clock::time_point::duration;
        // std::mktime returns date as local time zone. no conversion needed
        auto dt = std::chrono::system_clock::time_point(this->date);
        dt += std::chrono::duration_cast<internal_duration>(
                   std::chrono::nanoseconds(this->time));
        return dt;
    }

    operator std::time_t() const
    {
        return std::chrono::system_clock::to_time_t(
                std::chrono::system_clock::time_point(*this));
    }

    local_datetime() = default;
    ~local_datetime() = default;
    local_datetime(local_datetime const&) = default;
    local_datetime(local_datetime&&)      = default;
    local_datetime& operator=(local_datetime const&) = default;
    local_datetime& operator=(local_datetime&&)      = default;
};

inline bool operator==(const local_datetime& lhs, const local_datetime& rhs)
{
    return std::make_tuple(lhs.date, lhs.time) ==
           std::make_tuple(rhs.date, rhs.time);
}
inline bool operator!=(const local_datetime& lhs, const local_datetime& rhs)
{
    return !(lhs == rhs);
}
inline bool operator< (const local_datetime& lhs, const local_datetime& rhs)
{
    return std::make_tuple(lhs.date, lhs.time) <
           std::make_tuple(rhs.date, rhs.time);
}
inline bool operator<=(const local_datetime& lhs, const local_datetime& rhs)
{
    return (lhs < rhs) || (lhs == rhs);
}
inline bool operator> (const local_datetime& lhs, const local_datetime& rhs)
{
    return !(lhs <= rhs);
}
inline bool operator>=(const local_datetime& lhs, const local_datetime& rhs)
{
    return !(lhs < rhs);
}

template<typename charT, typename traits>
std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& os, const local_datetime& dt)
{
    os << dt.date << 'T' << dt.time;
    return os;
}

struct offset_datetime
{
    local_date  date;
    local_time  time;
    time_offset offset;

    offset_datetime(local_date d, local_time t, time_offset o)
        : date(d), time(t), offset(o)
    {}
    offset_datetime(const local_datetime& dt, time_offset o)
        : date(dt.date), time(dt.time), offset(o)
    {}
    explicit offset_datetime(const local_datetime& ld)
        : date(ld.date), time(ld.time), offset(get_local_offset())
    {}
    explicit offset_datetime(const std::chrono::system_clock::time_point& tp)
        : offset_datetime(local_datetime(tp))
    {}
    explicit offset_datetime(const std::time_t& t)
        : offset_datetime(local_datetime(t))
    {}
    explicit offset_datetime(const std::tm& t)
        : offset_datetime(local_datetime(t))
    {}

    operator std::chrono::system_clock::time_point() const
    {
        // get date-time
        using internal_duration =
            typename std::chrono::system_clock::time_point::duration;
        std::chrono::system_clock::time_point tp =
            std::chrono::system_clock::time_point(this->date) +
            std::chrono::duration_cast<internal_duration>(
                   std::chrono::nanoseconds(this->time));

        // get date-time in UTC. let's say we are in +09:00 (JPN).
        // writing 12:00:00 in +09:00 means 03:00:00Z. to represent
        // 12:00:00Z, first we need to add +09:00.
        const auto ofs = get_local_offset();
        tp += std::chrono::hours  (ofs.hour);
        tp += std::chrono::minutes(ofs.minute);

        // here, tp represents 12:00:00 in UTC but we have offset information.
        // we need to subtract it. For example, let's say the input is
        // 12:00:00-08:00. now we have tp = 12:00:00Z as a result of the above
        // conversion. But the actual time we need to return is 20:00:00Z
        // because of -08:00.
        tp -= std::chrono::minutes(this->offset);
        return tp;
    }

    operator std::time_t() const
    {
        return std::chrono::system_clock::to_time_t(
                std::chrono::system_clock::time_point(*this));
    }

    offset_datetime() = default;
    ~offset_datetime() = default;
    offset_datetime(offset_datetime const&) = default;
    offset_datetime(offset_datetime&&)      = default;
    offset_datetime& operator=(offset_datetime const&) = default;
    offset_datetime& operator=(offset_datetime&&)      = default;

  private:

    static time_offset get_local_offset()
    {
         // get current timezone
        const auto  tmp1 = std::time(nullptr);
        const auto  tmp2 = std::localtime(&tmp1); // XXX not threadsafe!
        assert(tmp2);
        std::tm t = *tmp2;

        std::array<char, 6> buf;
        const auto result = std::strftime(buf.data(), 6, "%z", &t); // +hhmm\0
        if(result != 5)
        {
            throw std::runtime_error("toml::offset_datetime: cannot obtain "
                                     "timezone information of current env");
        }
        const int ofs = std::atoi(buf.data());
        const int ofs_h = ofs / 100;
        const int ofs_m = ofs - (ofs_h * 100);
        return time_offset(ofs_h, ofs_m);
    }
};

inline bool operator==(const offset_datetime& lhs, const offset_datetime& rhs)
{
    return std::make_tuple(lhs.date, lhs.time, lhs.offset) ==
           std::make_tuple(rhs.date, rhs.time, rhs.offset);
}
inline bool operator!=(const offset_datetime& lhs, const offset_datetime& rhs)
{
    return !(lhs == rhs);
}
inline bool operator< (const offset_datetime& lhs, const offset_datetime& rhs)
{
    return std::make_tuple(lhs.date, lhs.time, lhs.offset) <
           std::make_tuple(rhs.date, rhs.time, rhs.offset);
}
inline bool operator<=(const offset_datetime& lhs, const offset_datetime& rhs)
{
    return (lhs < rhs) || (lhs == rhs);
}
inline bool operator> (const offset_datetime& lhs, const offset_datetime& rhs)
{
    return !(lhs <= rhs);
}
inline bool operator>=(const offset_datetime& lhs, const offset_datetime& rhs)
{
    return !(lhs < rhs);
}

template<typename charT, typename traits>
std::basic_ostream<charT, traits>&
operator<<(std::basic_ostream<charT, traits>& os, const offset_datetime& dt)
{
    os << dt.date << 'T' << dt.time << dt.offset;
    return os;
}

}//toml
#endif// TOML11_DATETIME
