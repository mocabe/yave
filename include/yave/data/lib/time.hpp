//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>
#include <yave/core/data_types/time/bounded_int.hpp>

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
    [[nodiscard]] static constexpr value_type per_millisecond() noexcept
    {
      return per_second() / 1000;
    }

    /// Unit per second.
    [[nodiscard]] static constexpr value_type per_second() noexcept
    {
      return ratio::den / ratio::num;
    }

    /// Unit per minute.
    [[nodiscard]] static constexpr value_type per_minute() noexcept
    {
      return per_second() * 60;
    }

    /// Unit per hour.
    [[nodiscard]] static constexpr value_type per_hour() noexcept
    {
      return per_minute() * 60;
    }

    /// Unit per day.
    [[nodiscard]] static constexpr value_type per_day() noexcept
    {
      return per_hour() * 24;
    }

    /// Get unit per rate.
    /// \param rps_num numerator of rate per second
    /// \param rps_den denominator of rate per second
    /// \return (per_second() * rps_den) / rps_num
    [[nodiscard]] static constexpr value_type
      per_rate(int64_t rps_num, int64_t rps_den = 1) noexcept
    {
      return (per_second() * rps_den) / rps_num;
    }

    /// Check rate compatibility.
    /// \param rps_num numerator of rate per second
    /// \param rps_den denominator of rate per second
    /// \return (per_second() * rps_den) % rps_num == 0
    [[nodiscard]] static constexpr bool
      is_compatible_rate(int64_t rps_num, int64_t rps_den = 1) noexcept
    {
      return (per_second() * rps_den) % rps_num == 0;
    }

    /// Maximum milliseconds can be contained.
    [[nodiscard]] static constexpr double max_milliseconds() noexcept
    {
      return static_cast<double>(std::numeric_limits<value_type>::max()) /
             static_cast<double>(per_millisecond());
    }

    /// Maximum seconds can be contained.
    [[nodiscard]] static constexpr double max_seconds() noexcept
    {
      return static_cast<double>(std::numeric_limits<value_type>::max()) /
             static_cast<double>(per_second());
    }

    /// Maximum minutes can be contained.
    [[nodiscard]] static constexpr double max_minutes() noexcept
    {
      return static_cast<double>(std::numeric_limits<value_type>::max()) /
             static_cast<double>(per_minute());
    }

    /// Maximum hours can be contained.
    [[nodiscard]] static constexpr double max_hours() noexcept
    {
      return static_cast<double>(std::numeric_limits<value_type>::max()) /
             static_cast<double>(per_hour());
    }

    /// Maximum days can be containd.
    [[nodiscard]] static constexpr double max_days() noexcept
    {
      return static_cast<double>(std::numeric_limits<value_type>::max()) /
             static_cast<double>(per_day());
    }

    /// Minimum millisecond can be contained (ca be negative).
    [[nodiscard]] static constexpr double min_milliseconds() noexcept
    {
      return static_cast<double>(std::numeric_limits<value_type>::min()) /
             static_cast<double>(per_millisecond());
    }

    /// Minimum seconds can be containd (can be negative).
    [[nodiscard]] static constexpr double min_seconds() noexcept
    {
      return static_cast<double>(std::numeric_limits<value_type>::min()) /
             static_cast<double>(per_second());
    }

    /// Minimum minutes can be contained (can be negative).
    [[nodiscard]] static constexpr double min_minutes() noexcept
    {
      return static_cast<double>(std::numeric_limits<value_type>::min()) /
             static_cast<double>(per_minute());
    }

    /// Minimum hours can be contained (can be negative).
    [[nodiscard]] static constexpr double min_hours() noexcept
    {
      return static_cast<double>(std::numeric_limits<value_type>::min()) /
             static_cast<double>(per_hour());
    }

    /// Minimum days can be contained (can be negative).
    [[nodiscard]] static constexpr double min_days() noexcept
    {
      return static_cast<double>(std::numeric_limits<value_type>::min()) /
             static_cast<double>(per_day());
    }

    /// Max time.
    [[nodiscard]] static constexpr time max() noexcept
    {
      return time {std::numeric_limits<value_type>::max()};
    }

    /// Min time (possibly negative).
    [[nodiscard]] static constexpr time min() noexcept
    {
      return time {std::numeric_limits<value_type>::lowest()};
    }

    /// Zero time point.
    [[nodiscard]] static constexpr time zero() noexcept
    {
      return time {0};
    }

    /// Create time from milliseconds.
    /// When specified time is out of the range which can be
    /// represented by this class, returns clamped value.
    [[nodiscard]] static constexpr time
      milliseconds(double millisecond) noexcept
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
    [[nodiscard]] static constexpr time seconds(double second) noexcept
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
    [[nodiscard]] static constexpr time minutes(double minute) noexcept
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
    [[nodiscard]] static constexpr time hours(double hour) noexcept
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
    [[nodiscard]] static constexpr time days(double day) noexcept
    {
      if (day >= max_days())
        return max();
      if (day <= min_days())
        return min();
      return time {static_cast<value_type>(day) * per_day()};
    }

    /// Convert to millisecond.
    [[nodiscard]] constexpr double to_millisecond() const noexcept
    {
      return static_cast<double>(m_value) /
             static_cast<double>(per_millisecond());
    }

    /// Convert to second.
    [[nodiscard]] constexpr double to_second() const noexcept
    {
      return static_cast<double>(m_value) / static_cast<double>(per_second());
    }

    /// Convert to minute.
    [[nodiscard]] constexpr double to_minute() const noexcept
    {
      return static_cast<double>(m_value) / static_cast<double>(per_minute());
    }

    /// Convert to hour.
    [[nodiscard]] constexpr double to_hour() const noexcept
    {
      return static_cast<double>(m_value) / static_cast<double>(per_hour());
    }

    /// Convert to day.
    [[nodiscard]] constexpr double to_day() const noexcept
    {
      return static_cast<double>(m_value) / static_cast<double>(per_day());
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
    [[nodiscard]] constexpr time operator+() const noexcept;
    /// operator-
    [[nodiscard]] constexpr time operator-() const noexcept;

    /// operator++
    constexpr time operator++() noexcept;
    /// operator+-
    constexpr time operator--() noexcept;

    /// operator++(int)
    constexpr time operator++(int) noexcept;
    /// operator--(int)
    constexpr time operator--(int) noexcept;

    /// Get int value.
    [[nodiscard]] constexpr value_type int_value() const noexcept
    {
      return m_value;
    }

  private:
    constexpr time(bounded_int<value_type> value)
      : m_value {value}
    {
    }

  private:
    /// value
    bounded_int<value_type> m_value;
  };

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
    return {+m_value};
  }

  constexpr time time::operator-() const noexcept
  {
    return {-m_value};
  }

  constexpr time time::operator++() noexcept
  {
    return {++m_value};
  }

  constexpr time time::operator--() noexcept
  {
    return {--m_value};
  }

  constexpr time time::operator++(int) noexcept
  {
    return {m_value++};
  }

  constexpr time time::operator--(int) noexcept
  {
    return {m_value--};
  }

  [[nodiscard]] constexpr time
    operator+(const time& lhs, const time& rhs) noexcept
  {
    return time {lhs} += rhs;
  }

  [[nodiscard]] constexpr time
    operator-(const time& lhs, const time& rhs) noexcept
  {
    return time {lhs} -= rhs;
  }

  [[nodiscard]] constexpr time
    operator*(const time& lhs, const time& rhs) noexcept
  {
    return time {lhs} *= rhs;
  }

  [[nodiscard]] constexpr time
    operator/(const time& lhs, const time& rhs) noexcept
  {
    return time {lhs} /= rhs;
  }

  [[nodiscard]] constexpr bool
    operator==(const time& lhs, const time& rhs) noexcept
  {
    return lhs.int_value() == rhs.int_value();
  }

  [[nodiscard]] constexpr bool
    operator!=(const time& lhs, const time& rhs) noexcept
  {
    return lhs.int_value() != rhs.int_value();
  }

  [[nodiscard]] constexpr bool
    operator<(const time& lhs, const time& rhs) noexcept
  {
    return lhs.int_value() < rhs.int_value();
  }

  [[nodiscard]] constexpr bool
    operator<=(const time& lhs, const time& rhs) noexcept
  {
    return lhs.int_value() <= rhs.int_value();
  }

  [[nodiscard]] constexpr bool
    operator>(const time& lhs, const time& rhs) noexcept
  {
    return lhs.int_value() > rhs.int_value();
  }

  [[nodiscard]] constexpr bool
    operator>=(const time& lhs, const time& rhs) noexcept
  {
    return lhs.int_value() >= rhs.int_value();
  }

} // namespace yave