//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/vec.hpp>

namespace yave::ui {

  // 2D point struct
  struct point
  {
    f64 x = 0;
    f64 y = 0;

    constexpr point() = default;

    constexpr point(f64 s)
      : x {s}
      , y {s}
    {
    }

    constexpr point(f64 x, f64 y)
      : x {x}
      , y {y}
    {
    }

    constexpr point(const vec& v)
      : x {v.x}
      , y {v.y}
    {
    }

    constexpr auto operator+=(const vec& o) -> point&
    {
      x += o.x;
      y += o.y;
      return *this;
    }

    constexpr auto operator-=(const vec& o) -> point&
    {
      x -= o.x;
      y -= o.y;
      return *this;
    }

    constexpr auto vec() const
    {
      return ui::vec(x, y);
    }

    constexpr bool operator==(const point&) const = default;
    constexpr bool operator!=(const point&) const = default;
  };

  constexpr auto operator+(const point& p, const vec o) -> point
  {
    auto r = p;
    r += o;
    return r;
  }

  constexpr auto operator-(const point& p, const vec o) -> point
  {
    auto r = p;
    r -= o;
    return r;
  }

  constexpr auto operator-(const point& p1, const point& p2) -> vec
  {
    return vec(p1.x - p2.x, p1.y - p2.y);
  }

} // namespace yave::ui