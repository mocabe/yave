//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <limits>

namespace yave {

  /// Non-overflowing signed integer value.
  template <class IntegerT>
  class bounded_int
  {
  public:
    using value_type = IntegerT;

    constexpr bounded_int(value_type v)
      : m_value {v}
    {
    }

    constexpr operator value_type() const noexcept
    {
      return m_value;
    }

    constexpr bounded_int& operator+=(const bounded_int& rhs) noexcept;
    constexpr bounded_int& operator-=(const bounded_int& rhs) noexcept;
    constexpr bounded_int& operator*=(const bounded_int& rhs) noexcept;
    constexpr bounded_int& operator/=(const bounded_int& rhs) noexcept;

    [[nodiscard]] constexpr bounded_int operator+() const noexcept;
    [[nodiscard]] constexpr bounded_int operator-() const noexcept;

    constexpr bounded_int& operator++() noexcept;
    constexpr bounded_int& operator--() noexcept;
    constexpr bounded_int operator++(int) noexcept;
    constexpr bounded_int operator--(int) noexcept;

  private:
    value_type m_value;
  };

  template <class T>
  constexpr bounded_int<T>& bounded_int<T>::
    operator+=(const bounded_int<T>& rhs) noexcept
  {

    if (m_value == 0) {
      m_value = rhs.m_value;
      return *this;
    }

    // [Eq: A + B > MAX]
    // (A, B) -> COND
    // (+, +) -> B > MAX - A
    // (+, -) -> -B > MAX - A
    // (-, +) -> -A > MAX - B  -> false
    // (-, -) -> -A > MAX - -B -> false
    if (m_value > 0) {
      if (std::numeric_limits<value_type>::max() - m_value < rhs.m_value) {
        m_value = std::numeric_limits<value_type>::max();
        return *this;
      }
    }

    // [Eq: A + B < MIN]
    // (A, B) -> COND
    // (+, +) -> B > MIN - A  -> false
    // (+, -) -> -B > MIN - A -> false
    // (-, +) -> B > MIN - -A
    // (-, -) -> -B > MIN - -A
    if (m_value < 0) {
      if (std::numeric_limits<value_type>::min() - m_value > rhs.m_value) {
        m_value = std::numeric_limits<value_type>::min();
        return *this;
      }
    }

    m_value += rhs.m_value;
    return *this;
  }

  template <class T>
  constexpr bounded_int<T>& bounded_int<T>::
    operator-=(const bounded_int<T>& rhs) noexcept
  {
    if (rhs.m_value == 0)
      return *this;

    // [Eq: A - B > MAX]
    // (A, B) -> COND
    // (+, +) -> A > MAX - B  -> false
    // (+, -) -> A > MAX + -B
    // (-, +) -> -A - B > MAX -> false
    // (-, -) -> -A > MAX + -B
    if (rhs.m_value < 0) {
      if (m_value > std::numeric_limits<value_type>::max() + rhs.m_value) {
        m_value = std::numeric_limits<value_type>::max();
        return *this;
      }
    }

    // [Eq: A - B < MIN]
    // (A, B) -> COND
    // (+, +) -> A < MIN + B
    // (+, -) -> A < MIN + -B  -> false
    // (-, +) -> -A < MIN + B
    // (-, -) -> -A < MIN + -B -> false
    if (rhs.m_value > 0) {
      if (m_value < std::numeric_limits<value_type>::min() + rhs.m_value) {
        m_value = std::numeric_limits<value_type>::min();
        return *this;
      }
    }

    m_value -= rhs.m_value;
    return *this;
  }

  template <class T>
  constexpr bounded_int<T>& bounded_int<T>::
    operator*=(const bounded_int<T>& rhs) noexcept
  {
    if (rhs.m_value == 0) {
      m_value = 0;
      return *this;
    }

    // [Eq: A * B > MAX]
    // (A, B) -> COND
    // (+, +) -> A > MAX / B
    // (+, -) -> A <= MAX / -B -> false
    // (-, +) -> -A > MAX / B  -> false
    // (-, -) -> -A <= MAX / -B

    // [Eq: A * B < MIN] 
    // (A, B) -> COND
    // (+, +) -> A < MIN / B    -> false
    // (+, -) -> -B < MIN / A
    // (-, +) -> -A < MIN / B 
    // (-, -) -> -A >= MIN / -B -> false

    // (+, +) -> A > MAX / B
    // (-, +) -> -A < MIN / B 
    if (rhs.m_value > 0) {
      if (m_value > std::numeric_limits<value_type>::max() / rhs.m_value) {
        m_value = std::numeric_limits<value_type>::max();
        return *this;
      }
      if (m_value < std::numeric_limits<value_type>::min() / rhs.m_value) {
        m_value = std::numeric_limits<value_type>::min();
        return *this;
      }
    }

    // (-, -) -> -A <= MAX / -B
    // (+, -) -> -B < MIN / A
    if (rhs.m_value < 0) {
      if (m_value <= std::numeric_limits<value_type>::max() / rhs.m_value) {
        m_value = std::numeric_limits<value_type>::max();
        return *this;
      }
      if (rhs.m_value < std::numeric_limits<value_type>::min() / m_value) {
        m_value = std::numeric_limits<value_type>::min();
        return *this;
      }
    }

    m_value *= rhs.m_value;
    return *this;
  }

  template <class T>
  constexpr bounded_int<T>& bounded_int<T>::
    operator/=(const bounded_int<T>& rhs) noexcept
  {
    if (rhs.m_value == 0) {
      m_value = std::numeric_limits<value_type>::max();
      return *this;
    }

    // [Eq: A / B > MAX]
    // (A, B) -> COND
    // (+, +) -> A / MAX > B
    // (+, -) -> A / MAX <= -B -> false
    // (-, +) -> -A / MAX > B  -> false
    // (-, -) -> -A / MAX <= -B

    // [Eq: A / B < MIN]
    // (A, B) -> COND
    // (+, +) -> A / MIN >= B  -> false
    // (+, -) -> A / MIN < -B
    // (-, +) -> -A / MIN >= B
    // (-, -) -> -A / MIN < -B -> false

    // (+, +) -> A / MAX > B
    // (-, +) -> -A / MIN >= B
    if (rhs.m_value > 0) {
      if (m_value / std::numeric_limits<value_type>::max() > rhs.m_value) {
        m_value = std::numeric_limits<value_type>::max();
        return *this;
      }
      if (m_value / std::numeric_limits<value_type>::min() >= rhs.m_value) {
        m_value = std::numeric_limits<value_type>::min();
        return *this;
      }
    }

    // (-, -) -> -A / MAX <= -B
    // (+, -) -> A / MIN < -B
    if (rhs.m_value < 0) {
      if (m_value / std::numeric_limits<value_type>::max() <= rhs.m_value) {
        m_value = std::numeric_limits<value_type>::max();
        return *this;
      }
      if (m_value / std::numeric_limits<value_type>::min() < rhs.m_value) {
        m_value = std::numeric_limits<value_type>::min();
        return *this;
      }
    }

    m_value /= rhs.m_value;
    return *this;
  }

  template <class T>
  [[nodiscard]] constexpr bounded_int<T> bounded_int<T>::operator+() const
    noexcept
  {
    return m_value;
  }

  template <class T>
  [[nodiscard]] constexpr bounded_int<T> bounded_int<T>::operator-() const
    noexcept
  {
    if (m_value == std::numeric_limits<value_type>::min())
      return std::numeric_limits<value_type>::max();
    return -m_value;
  }

  template <class T>
  constexpr bounded_int<T>& bounded_int<T>::operator++() noexcept
  {
    if (m_value != std::numeric_limits<value_type>::max())
      m_value += 1;
    return *this;
  }

  template <class T>
  constexpr bounded_int<T>& bounded_int<T>::operator--() noexcept
  {
    if (m_value != std::numeric_limits<value_type>::min())
      m_value -= 1;
    return *this;
  }

  template <class T>
  constexpr bounded_int<T> bounded_int<T>::operator++(int) noexcept
  {
    auto tmp = m_value;
    ++(*this);
    return tmp;
  }

  template <class T>
  constexpr bounded_int<T> bounded_int<T>::operator--(int) noexcept
  {
    auto tmp = m_value;
    --(*this);
    return tmp;
  }

  template <class T>
  [[nodiscard]] constexpr bounded_int<T>
    operator+(const bounded_int<T>& lhs, const bounded_int<T>& rhs) noexcept
  {
    return bounded_int<T> {lhs} += rhs;
  }

  template <class T>
  [[nodiscard]] constexpr bounded_int<T>
    operator-(const bounded_int<T>& lhs, const bounded_int<T>& rhs) noexcept
  {
    return bounded_int<T> {lhs} -= rhs;
  }

  template <class T>
  [[nodiscard]] constexpr bounded_int<T>
    operator*(const bounded_int<T>& lhs, const bounded_int<T>& rhs) noexcept
  {
    return bounded_int<T> {lhs} *= rhs;
  }

  template <class T>
  [[nodiscard]] constexpr bounded_int<T>
    operator/(const bounded_int<T>& lhs, const bounded_int<T>& rhs) noexcept
  {
    return bounded_int<T> {lhs} /= rhs;
  }

  template <class T>
  [[nodiscard]] constexpr bool
    operator==(const bounded_int<T>& lhs, const bounded_int<T>& rhs) noexcept
  {
    return lhs == rhs;
  }

  template <class T>
  [[nodiscard]] constexpr bool
    operator!=(const bounded_int<T>& lhs, const bounded_int<T>& rhs) noexcept
  {
    return lhs != rhs;
  }

  template <class T>
  [[nodiscard]] constexpr bool
    operator<(const bounded_int<T>& lhs, const bounded_int<T>& rhs) noexcept
  {
    return lhs < rhs;
  }

  template <class T>
  [[nodiscard]] constexpr bool
    operator<=(const bounded_int<T>& lhs, const bounded_int<T>& rhs) noexcept
  {
    return lhs <= rhs;
  }

  template <class T>
  [[nodiscard]] constexpr bool
    operator>(const bounded_int<T>& lhs, const bounded_int<T>& rhs) noexcept
  {
    return lhs > rhs;
  }

  template <class T>
  [[nodiscard]] constexpr bool
    operator>=(const bounded_int<T>& lhs, const bounded_int<T>& rhs) noexcept
  {
    return lhs >= rhs;
  }

}