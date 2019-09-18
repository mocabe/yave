//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#include <array>

namespace yave {

  /// dim 1 vector
  template <class T>
  struct tvec1
  {
    using value_type              = T;
    constexpr tvec1()             = default;
    constexpr tvec1(const tvec1&) = default;
    constexpr tvec1(tvec1&&)      = default;
    constexpr tvec1& operator=(const tvec1&) = default;
    constexpr tvec1& operator=(tvec1&&) = default;

    constexpr tvec1(T v)
      : values {v}
    {
    }

    [[nodiscard]] constexpr auto x() -> T&
    {
      return values[0];
    }

    [[nodiscard]] constexpr auto x() const -> const T&
    {
      return values[0];
    }

    [[nodiscard]] constexpr auto operator[](size_t i) -> T&
    {
      return values[i];
    }

    [[nodiscard]] constexpr auto operator[](size_t i) const -> const T&
    {
      return values[i];
    }

    constexpr auto operator+=(const tvec1& rhs) -> tvec1&
    {
      values[0] += rhs.values[0];
      return *this;
    }

    constexpr auto operator-=(const tvec1& rhs) -> tvec1&
    {
      values[0] -= rhs.values[0];
      return *this;
    }

    constexpr auto operator*=(const tvec1& rhs) -> tvec1&
    {
      values[0] *= rhs.values[0];
      return *this;
    }

    constexpr auto operator/=(const tvec1& rhs) -> tvec1&
    {
      values[0] /= rhs.values[0];
      return *this;
    }

  public:
    T values[1];
  };

  template <class T>
  [[nodiscard]] constexpr bool operator==(
    const tvec1<T>& lhs,
    const tvec1<T>& rhs)
  {
    return lhs.values[0] == rhs.values[0];
  }

  template <class T>
  [[nodiscard]] constexpr bool operator!=(
    const tvec1<T>& lhs,
    const tvec1<T>& rhs)
  {
    return !(lhs == rhs);
  }

  template <class T>
  [[nodiscard]] constexpr auto operator+(
    const tvec1<T>& lhs,
    const tvec1<T>& rhs) -> tvec1<T>
  {
    auto tmp = lhs;
    tmp += rhs;
    return tmp;
  }

  template <class T>
  [[nodiscard]] constexpr auto operator-(
    const tvec1<T>& lhs,
    const tvec1<T>& rhs) -> tvec1<T>
  {
    auto tmp = lhs;
    tmp -= rhs;
    return tmp;
  }

  template <class T>
  [[nodiscard]] constexpr auto operator*(
    const tvec1<T>& lhs,
    const tvec1<T>& rhs) -> tvec1<T>
  {
    auto tmp = lhs;
    tmp *= rhs;
    return tmp;
  }

  template <class T>
  [[nodiscard]] constexpr auto operator/(
    const tvec1<T>& lhs,
    const tvec1<T>& rhs) -> tvec1<T>
  {
    auto tmp = lhs;
    tmp /= rhs;
    return tmp;
  }
} // namespace yave
