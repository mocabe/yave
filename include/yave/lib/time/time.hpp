//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/lib/time/bounded_int.hpp>

#include <chrono>

namespace yave {

  /// Represents time point in audio/video/etc.
  /// time contains single integer value which can represent most of multimedia
  /// frame times.
  class time
  {
  public:
    /// value type
    using value_type = int64_t;

    /// std::chrono based duration ratio.
    /// Using "Flicks" time unit variant, found by Facebook.
    using ratio = std::ratio<1, 705600000LL * 4 / 5>;

    /// std::chrono based duration.
    using unit = std::chrono::duration<value_type, ratio>;

    /// Constructor.
    constexpr time() noexcept = default;

    /// Constructor.
    template <class T, class R>
    constexpr time(std::chrono::duration<T, R> v) noexcept
      : m_value {std::chrono::duration_cast<unit>(v).count()}
    {
    }

    /// operator=
    template <class T, class R>
    constexpr time& operator=(std::chrono::duration<T, R> v) noexcept
    {
      m_value = std::chrono::duration_cast<unit>(v).count();
      return *this;
    }

    /// Constructor.
    constexpr time(const time&) noexcept = default;
    /// Move constructor.
    constexpr time(time&&) noexcept = default;
    /// operator=
    constexpr time& operator=(const time&) noexcept = default;
    /// operator=
    constexpr time& operator=(time&&) noexcept = default;

    /// Get duration in unit.
    [[nodiscard]] constexpr auto duration() const noexcept -> unit
    {
      return unit(m_value);
    }

    /// Get integer value.
    [[nodiscard]] constexpr auto count() const noexcept -> value_type
    {
      return duration().count();
    }

    /// Convert to millisecond.
    [[nodiscard]] constexpr auto milliseconds() const noexcept
      -> std::chrono::duration<double, std::milli>
    {
      return std::chrono::duration_cast<
        std::chrono::duration<double, std::milli>>(duration());
    }

    /// Convert to second.
    [[nodiscard]] constexpr auto seconds() const noexcept
      -> std::chrono::duration<double>
    {
      return std::chrono::duration_cast<std::chrono::duration<double>>(
        duration());
    }

    /// Convert to minute.
    [[nodiscard]] constexpr auto minutes() const noexcept
      -> std::chrono::duration<double, std::ratio<60>>
    {
      return std::chrono::duration_cast<
        std::chrono::duration<double, std::ratio<60>>>(duration());
    }

    /// Convert to hour.
    [[nodiscard]] constexpr auto hours() const noexcept
      -> std::chrono::duration<double, std::ratio<60 * 60>>
    {
      return std::chrono::duration_cast<
        std::chrono::duration<double, std::ratio<60 * 60>>>(duration());
    }

    /// Unit per millisecond.
    [[nodiscard]] static constexpr unit per_millisecond() noexcept
    {
      return std::chrono::milliseconds(1);
    }

    /// Unit per second.
    [[nodiscard]] static constexpr unit per_second() noexcept
    {
      return std::chrono::seconds(1);
    }

    /// Unit per minute.
    [[nodiscard]] static constexpr unit per_minute() noexcept
    {
      return std::chrono::minutes(1);
    }

    /// Unit per hour.
    [[nodiscard]] static constexpr unit per_hour() noexcept
    {
      return std::chrono::hours(1);
    }

    /// Get unit per rate.
    /// \param rps_num numerator of rate per second
    /// \param rps_den denominator of rate per second
    /// \return (per_second() * rps_den) / rps_num
    [[nodiscard]] static constexpr unit per_rate(
      int64_t rps_num,
      int64_t rps_den = 1) noexcept
    {
      return (per_second() * rps_den) / rps_num;
    }

    /// Check rate compatibility.
    /// \param rps_num numerator of rate per second
    /// \param rps_den denominator of rate per second
    /// \return (per_second() * rps_den) % rps_num == 0
    [[nodiscard]] static constexpr bool is_compatible_rate(
      int64_t rps_num,
      int64_t rps_den = 1) noexcept
    {
      return (per_second() * rps_den) % rps_num == unit(0);
    }

    /// Maximum milliseconds can be contained.
    [[nodiscard]] static constexpr auto max_milliseconds() noexcept
      -> std::chrono::duration<double, std::milli>
    {
      return unit(std::numeric_limits<value_type>::max());
    }

    /// Maximum seconds can be contained.
    [[nodiscard]] static constexpr auto max_seconds() noexcept
      -> std::chrono::duration<double>
    {
      return unit(std::numeric_limits<value_type>::max());
    }

    /// Maximum minutes can be contained.
    [[nodiscard]] static constexpr auto max_minutes() noexcept
      -> std::chrono::duration<double, std::ratio<60>>
    {
      return unit(std::numeric_limits<value_type>::max());
    }

    /// Maximum hours can be contained.
    [[nodiscard]] static constexpr auto max_hours() noexcept
      -> std::chrono::duration<double, std::ratio<60 * 60>>
    {
      return unit(std::numeric_limits<value_type>::max());
    }

    /// Minimum millisecond can be contained (ca be negative).
    [[nodiscard]] static constexpr auto min_milliseconds() noexcept
      -> std::chrono::duration<double, std::milli>
    {
      return unit(std::numeric_limits<value_type>::lowest());
    }

    /// Minimum seconds can be containd (can be negative).
    [[nodiscard]] static constexpr auto min_seconds() noexcept
      -> std::chrono::duration<double>
    {
      return unit(std::numeric_limits<value_type>::lowest());
    }

    /// Minimum minutes can be contained (can be negative).
    [[nodiscard]] static constexpr auto min_minutes() noexcept
      -> std::chrono::duration<double, std::ratio<60>>
    {
      return unit(std::numeric_limits<value_type>::lowest());
    }

    /// Minimum hours can be contained (can be negative).
    [[nodiscard]] static constexpr auto min_hours() noexcept
      -> std::chrono::duration<double, std::ratio<60 * 60>>
    {
      return unit(std::numeric_limits<value_type>::lowest());
    }

    /// Max duration
    [[nodiscard]] static constexpr unit max_duration() noexcept
    {
      return unit(std::numeric_limits<value_type>::max());
    }

    /// Min duration
    [[nodiscard]] static constexpr unit min_duration() noexcept
    {
      return unit(std::numeric_limits<value_type>::lowest());
    }

    /// Zero duration
    [[nodiscard]] static constexpr unit zero_duration() noexcept
    {
      return unit(0);
    }

    /// Max time.
    [[nodiscard]] static constexpr time max() noexcept
    {
      return max_duration();
    }

    /// Min time (possibly negative).
    [[nodiscard]] static constexpr time min() noexcept
    {
      return min_duration();
    }

    /// Zero time point.
    [[nodiscard]] static constexpr time zero() noexcept
    {
      return zero_duration();
    }

    /// Create time from milliseconds.
    /// When specified time is out of the range which can be
    /// represented by this class, returns clamped value.
    template <class T>
    [[nodiscard]] static constexpr time milliseconds(T v) noexcept
    {
      auto d = std::chrono::duration<T, std::milli>(v);
      if (d >= max_milliseconds())
        return max();
      if (d <= min_milliseconds())
        return min();
      return std::chrono::duration_cast<unit>(d);
    }

    /// Create time from seconds.
    /// When specified time is out of the range which can be
    /// represented by this class, returns clamped value.
    template <class T>
    [[nodiscard]] static constexpr time seconds(T v) noexcept
    {
      auto d = std::chrono::duration<T>(v);
      if (d >= max_seconds())
        return max();
      if (d <= min_seconds())
        return min();
      return std::chrono::duration_cast<unit>(d);
    }

    /// Create time from minutes.
    /// When specified time is out of the range which can be
    /// represented by this class, returns clamped value.
    template <class T>
    [[nodiscard]] static constexpr time minutes(T v) noexcept
    {
      auto d = std::chrono::duration<T, std::ratio<60>>(v);
      if (d >= max_minutes())
        return max();
      if (d <= min_minutes())
        return min();
      return std::chrono::duration_cast<unit>(d);
    }

    /// Create time from hours.
    /// When specified time is out of the range which can be
    /// represented by this class, returns clamped value.
    template <class T>
    [[nodiscard]] static constexpr time hours(T v) noexcept
    {
      auto d = std::chrono::duration<T, std::ratio<60 * 60>>(v);
      if (d >= max_hours())
        return max();
      if (d <= min_hours())
        return min();
      return std::chrono::duration_cast<unit>(d);
    }

    /// operator+=.
    /// TODO: Signed integer overflow should be clamped.
    constexpr time& operator+=(const time& rhs) noexcept;
    /// operator-=.
    /// TODO: Signed integer overflow should be clamped.
    constexpr time& operator-=(const time& rhs) noexcept;

    /// operator*=
    template <class T>
    constexpr time& operator*=(const T& rhs) noexcept;
    /// operator/=
    template <class T>
    constexpr time& operator/=(const T& rhs) noexcept;

    /// operator+
    [[nodiscard]] constexpr time operator+() const noexcept;
    /// operator-
    [[nodiscard]] constexpr time operator-() const noexcept;

    /// operator++
    constexpr time& operator++() noexcept;
    /// operator+-
    constexpr time& operator--() noexcept;

    /// operator++(int)
    constexpr time operator++(int) noexcept;
    /// operator--(int)
    constexpr time operator--(int) noexcept;

  private:
    constexpr time(bounded_int<value_type> value)
      : m_value {value}
    {
    }

  private:
    /// value
    bounded_int<value_type> m_value = {};
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

  template <class T>
  constexpr time& time::operator*=(const T& rhs) noexcept
  {
    if constexpr (std::is_same_v<T, time>) {
      m_value *= rhs.m_value;
      return *this;
    } else if constexpr (std::is_integral_v<T>) {
      m_value *= rhs;
      return *this;
    } else if constexpr (std::is_floating_point_v<T>) {
      return *this = time(duration() * rhs);
    } else
      static_assert(false_v<T>, "Invalid operand for time::operator*=");
  }

  template <class T>
  constexpr time& time::operator/=(const T& rhs) noexcept
  {
    if constexpr (std::is_same_v<T, time>) {
      m_value /= rhs.m_value;
      return *this;
    } else if constexpr (std::is_integral_v<T>) {
      m_value /= rhs;
      return *this;
    } else if constexpr (std::is_floating_point_v<T>) {
      return *this = time(duration() / rhs);
    } else
      static_assert(false_v<T>, "Invalid operand for time::operator/=");
  }

  constexpr time time::operator+() const noexcept
  {
    return {+m_value};
  }

  constexpr time time::operator-() const noexcept
  {
    return {-m_value};
  }

  constexpr time& time::operator++() noexcept
  {
    ++m_value;
    return *this;
  }

  constexpr time& time::operator--() noexcept
  {
    --m_value;
    return *this;
  }

  constexpr time time::operator++(int) noexcept
  {
    return {m_value++};
  }

  constexpr time time::operator--(int) noexcept
  {
    return {m_value--};
  }

  [[nodiscard]] constexpr time operator+(
    const time& lhs,
    const time& rhs) noexcept
  {
    return time {lhs} += rhs;
  }

  [[nodiscard]] constexpr time operator-(
    const time& lhs,
    const time& rhs) noexcept
  {
    return time {lhs} -= rhs;
  }

  template <class T>
  [[nodiscard]] constexpr time operator*(
    const time& lhs,
    const T& rhs) noexcept
  {
    return time {lhs} *= rhs;
  }

  template <class T>
  [[nodiscard]] constexpr time operator/(
    const time& lhs,
    const T& rhs) noexcept
  {
    return time {lhs} /= rhs;
  }

  [[nodiscard]] constexpr bool operator==(
    const time& lhs,
    const time& rhs) noexcept
  {
    return lhs.count() == rhs.count();
  }

  [[nodiscard]] constexpr bool operator!=(
    const time& lhs,
    const time& rhs) noexcept
  {
    return lhs.count() != rhs.count();
  }

  [[nodiscard]] constexpr bool operator<(
    const time& lhs,
    const time& rhs) noexcept
  {
    return lhs.count() < rhs.count();
  }

  [[nodiscard]] constexpr bool operator<=(
    const time& lhs,
    const time& rhs) noexcept
  {
    return lhs.count() <= rhs.count();
  }

  [[nodiscard]] constexpr bool operator>(
    const time& lhs,
    const time& rhs) noexcept
  {
    return lhs.count() > rhs.count();
  }

  [[nodiscard]] constexpr bool operator>=(
    const time& lhs,
    const time& rhs) noexcept
  {
    return lhs.count() >= rhs.count();
  }

} // namespace yave