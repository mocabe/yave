//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/vec.hpp>

namespace yave::ui {

  /// 2D size data
  struct size
  {
    f64 w = 0;
    f64 h = 0;

    constexpr size() = default;

    constexpr size(f64 s)
      : w {s}
      , h {w}
    {
    }

    constexpr size(f64 w, f64 h)
      : w {w}
      , h {h}
    {
    }

    constexpr size(const vec& v)
      : w {v.x}
      , h {v.y}
    {
    }

    constexpr auto vec() const
    {
      return ui::vec(w, h);
    }

    constexpr auto operator*=(f64 s) -> size&
    {
      w *= s;
      h *= s;
      return *this;
    }

    constexpr auto operator/=(f64 s) -> size&
    {
      w /= s;
      h /= s;
      return *this;
    }

    constexpr bool operator==(const size&) const = default;
    constexpr bool operator!=(const size&) const = default;
  };

  constexpr auto operator*(const size& sz, f64 s) -> size
  {
    auto tmp = sz;
    tmp *= s;
    return tmp;
  }

  constexpr auto operator/(const size& sz, f64 s) -> size
  {
    auto tmp = sz;
    tmp /= s;
    return tmp;
  }
}