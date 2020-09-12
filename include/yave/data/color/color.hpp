//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#include <glm/glm.hpp>

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

    color(const glm::dvec4& vec)
      : color(vec.x, vec.y, vec.z, vec.w)
    {
    }

    color(const glm::fvec4& vec)
      : color(vec.x, vec.y, vec.z, vec.w)
    {
    }

    operator glm::dvec4() const
    {
      return {r, g, b, a};
    }

    operator glm::fvec4() const
    {
      return {r, g, b, a};
    }
  };

  [[nodiscard]] inline bool operator==(const color& lhs, const color& rhs)
  {
    return glm::dvec4(lhs) == glm::dvec4(rhs);
  }

  [[nodiscard]] inline bool operator!=(const color& lhs, const color& rhs)
  {
    return !(lhs == rhs);
  }
} // namespace yave::data