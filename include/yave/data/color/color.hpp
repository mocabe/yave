//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/lib/vec/vec.hpp>

namespace yave::data {

  /// for Color
  struct color
  {
    double r = 0;
    double g = 0;
    double b = 0;
    double a = 0;

    color() = default;

    color(double r, double g, double b, double a)
      : r {r}
      , g {g}
      , b {b}
      , a {a}
    {
    }

    color(const dvec4& vec)
      : color(vec.x, vec.y, vec.z, vec.w)
    {
    }

    color(const fvec4& vec)
      : color(vec.x, vec.y, vec.z, vec.w)
    {
    }

    operator dvec4() const
    {
      return {r, g, b, a};
    }

    operator fvec4() const
    {
      return {r, g, b, a};
    }
  };

  [[nodiscard]] inline bool operator==(const color& lhs, const color& rhs)
  {
    return dvec4(lhs) == dvec4(rhs);
  }

  [[nodiscard]] inline bool operator!=(const color& lhs, const color& rhs)
  {
    return !(lhs == rhs);
  }
} // namespace yave::data