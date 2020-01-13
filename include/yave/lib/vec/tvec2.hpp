//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#include <array>

namespace yave {

  /// dim 2 vector
  template <class T>
  struct tvec2
  {
    using value_type              = T;
    constexpr tvec2()             = default;
    constexpr tvec2(const tvec2&) = default;
    constexpr tvec2(tvec2&&)      = default;
    constexpr tvec2& operator=(const tvec2&) = default;
    constexpr tvec2& operator=(tvec2&&) = default;

    constexpr tvec2(T v)
      : values {v, v}
    {
    }

    constexpr tvec2(T x, T y)
      : values {x, y}
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

    [[nodiscard]] constexpr auto y() -> T&
    {
      return values[1];
    }

    [[nodiscard]] constexpr auto y() const -> const T&
    {
      return values[1];
    }

    [[nodiscard]] constexpr auto operator[](size_t i) -> T&
    {
      return values[i];
    }

    [[nodiscard]] constexpr auto operator[](size_t i) const -> const T&
    {
      return values[i];
    }

    constexpr auto operator+=(const tvec2& rhs) -> tvec2&
    {
      values[0] += rhs.values[0];
      values[1] += rhs.values[1];
      return *this;
    }

    constexpr auto operator-=(const tvec2& rhs) -> tvec2&
    {
      values[0] -= rhs.values[0];
      values[1] -= rhs.values[1];
      return *this;
    }

    constexpr auto operator*=(const tvec2& rhs) -> tvec2&
    {
      values[0] *= rhs.values[0];
      values[1] *= rhs.values[1];
      return *this;
    }

    constexpr auto operator/=(const tvec2& rhs) -> tvec2&
    {
      values[0] /= rhs.values[0];
      values[1] /= rhs.values[1];
      return *this;
    }

  public:
    T values[2] = {};
  };

  template <class T>
  [[nodiscard]] constexpr bool operator==(
    const tvec2<T>& lhs,
    const tvec2<T>& rhs)
  {
    return lhs.values[0] == rhs.values[0] && //
           lhs.values[1] == rhs.values[1];   //
  }

  template <class T>
  [[nodiscard]] constexpr bool operator!=(
    const tvec2<T>& lhs,
    const tvec2<T>& rhs)
  {
    return !(lhs == rhs);
  }

  template <class T>
  [[nodiscard]] constexpr auto operator+(
    const tvec2<T>& lhs,
    const tvec2<T>& rhs) -> tvec2<T>
  {
    auto tmp = lhs;
    tmp += rhs;
    return tmp;
  }

  template <class T>
  [[nodiscard]] constexpr auto operator-(
    const tvec2<T>& lhs,
    const tvec2<T>& rhs) -> tvec2<T>
  {
    auto tmp = lhs;
    tmp -= rhs;
    return tmp;
  }

  template <class T>
  [[nodiscard]] constexpr auto operator*(
    const tvec2<T>& lhs,
    const tvec2<T>& rhs) -> tvec2<T>
  {
    auto tmp = lhs;
    tmp *= rhs;
    return tmp;
  }

  template <class T>
  [[nodiscard]] constexpr auto operator/(
    const tvec2<T>& lhs,
    const tvec2<T>& rhs) -> tvec2<T>
  {
    auto tmp = lhs;
    tmp /= rhs;
    return tmp;
  }
}