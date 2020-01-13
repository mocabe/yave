//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#include <array>

namespace yave {

  /// dim 3 vector
  template <class T>
  struct tvec3
  {
    using value_type              = T;
    constexpr tvec3()             = default;
    constexpr tvec3(const tvec3&) = default;
    constexpr tvec3(tvec3&&)      = default;
    constexpr tvec3& operator=(const tvec3&) = default;
    constexpr tvec3& operator=(tvec3&&) = default;

    constexpr tvec3(T v)
      : values {v, v, v}
    {
    }

    constexpr tvec3(T x, T y, T z)
      : values {x, y, z}
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

    [[nodiscard]] constexpr auto operator[](size_t i) -> T&
    {
      return values[i];
    }

    [[nodiscard]] constexpr auto operator[](size_t i) const -> const T&
    {
      return values[i];
    }

    constexpr auto operator+=(const tvec3& rhs) -> tvec3&
    {
      values[0] += rhs.values[0];
      values[1] += rhs.values[1];
      values[2] += rhs.values[2];
      return *this;
    }

    constexpr auto operator-=(const tvec3& rhs) -> tvec3&
    {
      values[0] -= rhs.values[0];
      values[1] -= rhs.values[1];
      values[2] -= rhs.values[2];
      return *this;
    }

    constexpr auto operator*=(const tvec3& rhs) -> tvec3&
    {
      values[0] *= rhs.values[0];
      values[1] *= rhs.values[1];
      values[2] *= rhs.values[2];
      return *this;
    }

    constexpr auto operator/=(const tvec3& rhs) -> tvec3&
    {
      values[0] /= rhs.values[0];
      values[1] /= rhs.values[1];
      values[2] /= rhs.values[2];
      return *this;
    }

  public:
    T values[3] = {};
  };

  template <class T>
  [[nodiscard]] constexpr bool operator==(
    const tvec3<T>& lhs,
    const tvec3<T>& rhs)
  {
    return lhs.values[0] == rhs.values[0] && //
           lhs.values[1] == rhs.values[1] && //
           lhs.values[2] == rhs.values[2];   //
  }

  template <class T>
  [[nodiscard]] constexpr bool operator!=(
    const tvec3<T>& lhs,
    const tvec3<T>& rhs)
  {
    return !(lhs == rhs);
  }

  template <class T>
  [[nodiscard]] constexpr auto operator+(
    const tvec3<T>& lhs,
    const tvec3<T>& rhs) -> tvec3<T>
  {
    auto tmp = lhs;
    tmp += rhs;
    return tmp;
  }

  template <class T>
  [[nodiscard]] constexpr auto operator-(
    const tvec3<T>& lhs,
    const tvec3<T>& rhs) -> tvec3<T>
  {
    auto tmp = lhs;
    tmp -= rhs;
    return tmp;
  }

  template <class T>
  [[nodiscard]] constexpr auto operator*(
    const tvec3<T>& lhs,
    const tvec3<T>& rhs) -> tvec3<T>
  {
    auto tmp = lhs;
    tmp *= rhs;
    return tmp;
  }

  template <class T>
  [[nodiscard]] constexpr auto operator/(
    const tvec3<T>& lhs,
    const tvec3<T>& rhs) -> tvec3<T>
  {
    auto tmp = lhs;
    tmp /= rhs;
    return tmp;
  }
} // namespace yave