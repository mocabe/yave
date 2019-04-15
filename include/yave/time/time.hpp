//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#include <chrono>

namespace yave {

  /// Represents time point in audio/video/etc.
  /// time contains single integer value.
  /// To convert the value to real time value(like seconds), you can use
  /// `to_*()` functions.
  class time
  {
  public:
    /// value type
    using value_type = int64_t;

    /// std::chrono based duration ratio.
    /// Using "Flicks" time unit variant, found by Facebook.
    using ratio = std::ratio<1, 705600000LL * 4 / 5>;

    /// std::chrono based duration.
    using duration = std::chrono::duration<value_type, ratio>;

    /// Unit per millisecond.
    static constexpr value_type per_millisecond() noexcept
    {
      return per_second() / 1000;
    }

    /// Unit per second.
    static constexpr value_type per_second() noexcept
    {
      return ratio::den / ratio::num;
    }

    /// Unit per minute.
    static constexpr value_type per_minute() noexcept
    {
      return per_second() * 60;
    }

    /// Unit per hour.
    static constexpr value_type per_hour() noexcept
    {
      return per_minute() * 60;
    }

    /// Unit per day.
    static constexpr value_type per_day() noexcept
    {
      return per_hour() * 24;
    }

    /// Get unit per rate.
    /// \param rps_num numerator of rate per second
    /// \param rps_den denominator of rate per second
    /// \return (per_second() * rps_den) / rps_num
    static constexpr value_type
      per_rate(int64_t rps_num, int64_t rps_den = 1) noexcept
    {
      return (per_second() * rps_den) / rps_num;
    }

    /// Check rate compatibility.
    /// \param rps_num numerator of rate per second
    /// \param rps_den denominator of rate per second
    /// \return (per_second() * rps_den) % rps_num == 0
    static constexpr bool
      is_compatible_rate(int64_t rps_num, int64_t rps_den = 1) noexcept
    {
      return (per_second() * rps_den) % rps_num == 0;
    }

    /// Maximum milliseconds can be contained.
    static constexpr double max_milliseconds() noexcept
    {
      return std::numeric_limits<value_type>::max() /
             static_cast<double>(per_millisecond());
    }

    /// Maximum seconds can be contained.
    static constexpr double max_seconds() noexcept
    {
      return std::numeric_limits<value_type>::max() /
             static_cast<double>(per_second());
    }

    /// Maximum minutes can be contained.
    static constexpr double max_minutes() noexcept
    {
      return std::numeric_limits<value_type>::max() /
             static_cast<double>(per_minute());
    }

    /// Maximum hours can be contained.
    static constexpr double max_hours() noexcept
    {
      return std::numeric_limits<value_type>::max() /
             static_cast<double>(per_hour());
    }

    /// Maximum days can be containd.
    static constexpr double max_days() noexcept
    {
      return std::numeric_limits<value_type>::max() /
             static_cast<double>(per_day());
    }

    /// Minimum millisecond can be contained (ca be negative).
    static constexpr double min_milliseconds() noexcept
    {
      return std::numeric_limits<value_type>::min() /
             static_cast<double>(per_millisecond());
    }

    /// Minimum seconds can be containd (can be negative).
    static constexpr double min_seconds() noexcept
    {
      return std::numeric_limits<value_type>::min() /
             static_cast<double>(per_second());
    }

    /// Minimum minutes can be contained (can be negative).
    static constexpr double min_minutes() noexcept
    {
      return std::numeric_limits<value_type>::min() /
             static_cast<double>(per_minute());
    }

    /// Minimum hours can be contained (can be negative).
    static constexpr double min_hours() noexcept
    {
      return std::numeric_limits<value_type>::min() /
             static_cast<double>(per_hour());
    }

    /// Minimum days can be contained (can be negative).
    static constexpr double min_days() noexcept
    {
      return std::numeric_limits<value_type>::min() /
             static_cast<double>(per_day());
    }

    /// Max time.
    static constexpr time max() noexcept
    {
      return time {std::numeric_limits<value_type>::max()};
    }

    /// Min time (possibly negative).
    static constexpr time min() noexcept
    {
      return time {std::numeric_limits<value_type>::lowest()};
    }

    /// Zero time point.
    static constexpr time zero() noexcept
    {
      return time {0};
    }

    /// Create time from milliseconds.
    /// When specified time is out of the range which can be
    /// represented by this class, returns clamped value.
    static constexpr time milliseconds(double millisecond) noexcept
    {
      if (millisecond >= max_milliseconds())
        return max();
      if (millisecond <= min_milliseconds())
        return min();
      return time {static_cast<value_type>(millisecond) * per_millisecond()};
    }

    /// Create time from seconds.
    /// When specified time is out of the range which can be
    /// represented by this class, returns clamped value.
    static constexpr time seconds(double second) noexcept
    {
      if (second >= max_seconds())
        return max();
      if (second <= min_seconds())
        return min();
      return time {static_cast<value_type>(second) * per_second()};
    }

    /// Create time from minutes.
    /// When specified time is out of the range which can be
    /// represented by this class, returns clamped value.
    static constexpr time minutes(double minute) noexcept
    {
      if (minute >= max_minutes())
        return max();
      if (minute <= min_minutes())
        return min();
      return time {static_cast<value_type>(minute) * per_minute()};
    }

    /// Create time from hours.
    /// When specified time is out of the range which can be
    /// represented by this class, returns clamped value.
    static constexpr time hours(double hour) noexcept
    {
      if (hour >= max_hours())
        return max();
      if (hour <= min_hours())
        return min();
      return time {static_cast<value_type>(hour) * per_hour()};
    }

    /// Create time from days.
    /// When specified time is out of the range which can be
    /// represented by this class, returns clamped value.
    static constexpr time days(double day) noexcept
    {
      if (day >= max_days())
        return max();
      if (day <= min_days())
        return min();
      return time {static_cast<value_type>(day) * per_day()};
    }

    /// Convert to millisecond.
    constexpr double to_millisecond() const noexcept
    {
      return m_value / static_cast<double>(per_millisecond());
    }

    /// Convert to second.
    constexpr double to_second() const noexcept
    {
      return m_value / static_cast<double>(per_second());
    }

    /// Convert to minute.
    constexpr double to_minute() const noexcept
    {
      return m_value / static_cast<double>(per_minute());
    }

    /// Convert to hour.
    constexpr double to_hour() const noexcept
    {
      return m_value / static_cast<double>(per_hour());
    }

    /// Convert to day.
    constexpr double to_day() const noexcept
    {
      return m_value / static_cast<double>(per_day());
    }

    /// Constructor.
    constexpr time() noexcept
      : m_value {0}
    {
    }
    /// Constructor.
    constexpr time(const time& t) noexcept
      : m_value {t.m_value}
    {
    }
    /// Constructor.
    constexpr time(value_type v) noexcept
      : m_value {v}
    {
    }

    /// operator=
    constexpr time& operator=(const time& t) noexcept
    {
      m_value = t.m_value;
      return *this;
    }

    /// operator=
    constexpr time& operator=(value_type v) noexcept
    {
      m_value = v;
      return *this;
    }

    /// operator+=.
    /// TODO: Signed integer overflow should be clamped.
    constexpr time& operator+=(const time& rhs) noexcept;
    /// operator-=.
    /// TODO: Signed integer overflow should be clamped.
    constexpr time& operator-=(const time& rhs) noexcept;
    /// operator*=
    constexpr time& operator*=(const time& rhs) noexcept;
    /// operator/=
    constexpr time& operator/=(const time& rhs) noexcept;
    /// operator+
    constexpr time operator+() const noexcept;
    /// operator-
    constexpr time operator-() const noexcept;

    /// Get int value.
    constexpr value_type int_value() const noexcept
    {
      return m_value;
    }

  private:
    /// value
    value_type m_value;
  };

  /* common video fps */

  static_assert(time::is_compatible_rate(1));
  static_assert(time::is_compatible_rate(2));
  static_assert(time::is_compatible_rate(3));
  static_assert(time::is_compatible_rate(5));
  static_assert(time::is_compatible_rate(10));
  static_assert(time::is_compatible_rate(15));
  static_assert(time::is_compatible_rate(24));
  static_assert(time::is_compatible_rate(25));
  static_assert(time::is_compatible_rate(30));
  static_assert(time::is_compatible_rate(32));
  static_assert(time::is_compatible_rate(48));
  static_assert(time::is_compatible_rate(50));
  static_assert(time::is_compatible_rate(60));
  static_assert(time::is_compatible_rate(64));
  static_assert(time::is_compatible_rate(90));
  static_assert(time::is_compatible_rate(96));

  /* high-end video fps */

  static_assert(time::is_compatible_rate(100));
  static_assert(time::is_compatible_rate(120));
  static_assert(time::is_compatible_rate(144));
  static_assert(time::is_compatible_rate(180));
  static_assert(time::is_compatible_rate(192));
  static_assert(time::is_compatible_rate(240));

  /* NTSC fps */

  static_assert(time::is_compatible_rate(1000 * 24, 1001));
  // static_assert(time::is_compatible_rate(1000 * 25, 1001));  Not supported!
  static_assert(time::is_compatible_rate(1000 * 30, 1001));
  static_assert(time::is_compatible_rate(1000 * 60, 1001));
  static_assert(time::is_compatible_rate(1000 * 120, 1001));

  /* common audio frequency */

  static_assert(time::is_compatible_rate(1000));
  static_assert(time::is_compatible_rate(4000));
  static_assert(time::is_compatible_rate(8000));
  static_assert(time::is_compatible_rate(16000));
  static_assert(time::is_compatible_rate(22050));
  static_assert(time::is_compatible_rate(24000));
  static_assert(time::is_compatible_rate(32000));
  static_assert(time::is_compatible_rate(44100));
  static_assert(time::is_compatible_rate(48000));
  static_assert(time::is_compatible_rate(88200));
  static_assert(time::is_compatible_rate(96000));
  static_assert(time::is_compatible_rate(192000));

  /* high-end audio frequency */

  static_assert(time::is_compatible_rate(352800));
  static_assert(time::is_compatible_rate(384000));
  static_assert(time::is_compatible_rate(768000));

  constexpr time& time::operator+=(const time& rhs) noexcept
  {
    m_value += rhs.m_value;
    return *this;
  }

  constexpr time& time::operator-=(const time& rhs) noexcept
  {
    m_value -= rhs.m_value;
    return *this;
  }

  constexpr time& time::operator*=(const time& rhs) noexcept
  {
    m_value *= rhs.m_value;
    return *this;
  }

  constexpr time& time::operator/=(const time& rhs) noexcept
  {
    m_value /= rhs.m_value;
    return *this;
  }

  constexpr time time::operator+() const noexcept
  {
    return time {+m_value};
  }

  constexpr time time::operator-() const noexcept
  {
    return time {-m_value};
  }

  constexpr time operator+(const time& lhs, const time& rhs) noexcept
  {
    return time {lhs} += rhs;
  }

  constexpr time operator-(const time& lhs, const time& rhs) noexcept
  {
    return time {lhs} -= rhs;
  }

  constexpr time operator*(const time& lhs, const time& rhs) noexcept
  {
    return time {lhs} *= rhs;
  }

  constexpr time operator/(const time& lhs, const time& rhs) noexcept
  {
    return time {lhs} /= rhs;
  }

  constexpr bool operator==(const time& lhs, const time& rhs) noexcept
  {
    return lhs.int_value() == rhs.int_value();
  }

  constexpr bool operator!=(const time& lhs, const time& rhs) noexcept
  {
    return lhs.int_value() != rhs.int_value();
  }

  constexpr bool operator<(const time& lhs, const time& rhs) noexcept
  {
    return lhs.int_value() < rhs.int_value();
  }

  constexpr bool operator<=(const time& lhs, const time& rhs) noexcept
  {
    return lhs.int_value() <= rhs.int_value();
  }

  constexpr bool operator>(const time& lhs, const time& rhs) noexcept
  {
    return lhs.int_value() > rhs.int_value();
  }

  constexpr bool operator>=(const time& lhs, const time& rhs) noexcept
  {
    return lhs.int_value() >= rhs.int_value();
  }

} // namespace yave