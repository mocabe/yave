//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/typedefs.hpp>

namespace yave::ui {

  /// 2D vector value
  struct vec
  {
    f64 x = 0;
    f64 y = 0;

    constexpr vec() = default;

    constexpr vec(f64 s)
      : x {s}
      , y {s}
    {
    }

    constexpr vec(f64 x, f64 y)
      : x {x}
      , y {y}
    {
    }

    constexpr vec& operator+=(const vec& o)
    {
      x += o.x;
      y += o.y;
      return *this;
    }

    constexpr vec& operator+=(f64 d)
    {
      x += d;
      y += d;
      return *this;
    }

    constexpr vec& operator-=(const vec& o)
    {
      x -= o.x;
      y -= o.y;
      return *this;
    }

    constexpr vec& operator-=(f64 d)
    {
      x -= d;
      y -= d;
      return *this;
    }

    constexpr vec& operator*=(const vec& v)
    {
      x *= v.x;
      y *= v.y;
      return *this;
    }

    constexpr vec& operator*=(f64 r)
    {
      x *= r;
      y *= r;
      return *this;
    }

    constexpr vec& operator/=(const vec& v)
    {
      x /= v.x;
      y /= v.y;
      return *this;
    }

    constexpr vec& operator/=(f64 r)
    {
      x /= r;
      y /= r;
      return *this;
    }

    constexpr bool operator==(const vec&) const = default;
    constexpr bool operator!=(const vec&) const = default;
  };

  constexpr auto operator+(const vec& v1, const vec& v2) -> vec
  {
    auto r = v1;
    r += v2;
    return r;
  }

  constexpr auto operator+(const vec& v, f64 d) -> vec
  {
    auto r = v;
    r += d;
    return r;
  }

  constexpr auto operator-(const vec& v1, const vec& v2) -> vec
  {
    auto r = v1;
    r -= v2;
    return r;
  }

  constexpr auto operator-(const vec& v1, f64 d) -> vec
  {
    auto r = v1;
    r -= d;
    return r;
  }

  constexpr auto operator-(const vec& v)
  {
    return vec {-v.x, -v.y};
  }

  constexpr auto operator*(const vec& v1, const vec& v2) -> vec
  {
    auto r = v1;
    r *= v2;
    return r;
  }

  constexpr auto operator*(const vec& v1, f64 d) -> vec
  {
    auto r = v1;
    r *= d;
    return r;
  }

  constexpr auto operator/(const vec& v1, const vec& v2) -> vec
  {
    auto r = v1;
    r /= v2;
    return r;
  }

  constexpr auto operator/(const vec& v1, f64 d) -> vec
  {
    auto r = v1;
    r /= d;
    return r;
  }

} // namespace yave::ui