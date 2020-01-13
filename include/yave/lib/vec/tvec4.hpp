//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#include <array>

namespace yave {

  /// dim 4 vector
  template <class T>
  struct tvec4
  {
    using value_type              = T;
    constexpr tvec4()             = default;
    constexpr tvec4(const tvec4&) = default;
    constexpr tvec4(tvec4&&)      = default;
    constexpr tvec4& operator=(const tvec4&) = default;
    constexpr tvec4& operator=(tvec4&&) = default;

    constexpr tvec4(T v)
      : values {v, v, v, v}
    {
    }

    constexpr tvec4(T x, T y, T z, T w)
      : values {x, y, z, w}
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

    [[nodiscard]] constexpr auto z() -> T&
    {
      return values[2];
    }

    [[nodiscard]] constexpr auto z() const -> const T&

    {
      return values[2];
    }

    [[nodiscard]] constexpr auto w() -> T&
    {
      return values[3];
    }

    [[nodiscard]] constexpr auto w() const -> const T&
    {
      return values[3];
    }

    [[nodiscard]] constexpr auto operator[](size_t i) -> T&
    {
      return values[i];
    }

    [[nodiscard]] constexpr auto operator[](size_t i) const -> const T&
    {
      return values[i];
    }

    constexpr auto operator+=(const tvec4& rhs) -> tvec4&
    {
      values[0] += rhs.values[0];
      values[1] += rhs.values[1];
      values[2] += rhs.values[2];
      values[3] += rhs.values[3];
      return *this;
    }

    constexpr auto operator-=(const tvec4& rhs) -> tvec4&
    {
      values[0] -= rhs.values[0];
      values[1] -= rhs.values[1];
      values[2] -= rhs.values[2];
      values[3] -= rhs.values[3];
      return *this;
    }

    constexpr auto operator*=(const tvec4& rhs) -> tvec4&
    {
      values[0] *= rhs.values[0];
      values[1] *= rhs.values[1];
      values[2] *= rhs.values[2];
      values[3] *= rhs.values[3];
      return *this;
    }

    constexpr auto operator/=(const tvec4& rhs) -> tvec4&
    {
      values[0] /= rhs.values[0];
      values[1] /= rhs.values[1];
      values[2] /= rhs.values[2];
      values[3] /= rhs.values[3];
      return *this;
    }

  public:
    T values[4] = {};
  };

  template <class T>
  [[nodiscard]] constexpr bool operator==(
    const tvec4<T>& lhs,
    const tvec4<T>& rhs)
  {
    return lhs.values[0] == rhs.values[0] && //
           lhs.values[1] == rhs.values[1] && //
           lhs.values[2] == rhs.values[2] && //
           lhs.values[3] == rhs.values[3];   //
  }

  template <class T>
  [[nodiscard]] constexpr bool operator!=(
    const tvec4<T>& lhs,
    const tvec4<T>& rhs)
  {
    return !(lhs == rhs);
  }

  template <class T>
  [[nodiscard]] constexpr auto operator+(
    const tvec4<T>& lhs,
    const tvec4<T>& rhs) -> tvec4<T>
  {
    auto tmp = lhs;
    tmp += rhs;
    return tmp;
  }

  template <class T>
  [[nodiscard]] constexpr auto operator-(
    const tvec4<T>& lhs,
    const tvec4<T>& rhs) -> tvec4<T>
  {
    auto tmp = lhs;
    tmp -= rhs;
    return tmp;
  }

  template <class T>
  [[nodiscard]] constexpr auto operator*(
    const tvec4<T>& lhs,
    const tvec4<T>& rhs) -> tvec4<T>
  {
    auto tmp = lhs;
    tmp *= rhs;
    return tmp;
  }

  template <class T>
  [[nodiscard]] constexpr auto operator/(
    const tvec4<T>& lhs,
    const tvec4<T>& rhs) -> tvec4<T>
  {
    auto tmp = lhs;
    tmp /= rhs;
    return tmp;
  }
}