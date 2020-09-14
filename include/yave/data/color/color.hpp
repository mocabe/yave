//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#include <glm/glm.hpp>

namespace yave::data {

  /// color data
  struct color
  {
    float r = 0;
    float g = 0;
    float b = 0;
    float a = 0;

    color() = default;

    color(float r, float g, float b, float a)
      : r {r}
      , g {g}
      , b {b}
      , a {a}
    {
    }

    color(double r, double g, double b, double a)
      : r {static_cast<float>(r)}
      , g {static_cast<float>(g)}
      , b {static_cast<float>(b)}
      , a {static_cast<float>(a)}
    {
    }

    color(const glm::fvec4& vec)
      : color(vec.x, vec.y, vec.z, vec.w)
    {
    }

    color(const glm::dvec4& vec)
      : color(vec.x, vec.y, vec.z, vec.w)
    {
    }

    operator glm::fvec4() const
    {
      return {r, g, b, a};
    }

    operator glm::dvec4() const
    {
      return {r, g, b, a};
    }
  };

  [[nodiscard]] inline bool operator==(const color& lhs, const color& rhs)
  {
    return glm::fvec4(lhs) == glm::fvec4(rhs);
  }

  [[nodiscard]] inline bool operator!=(const color& lhs, const color& rhs)
  {
    return !(lhs == rhs);
  }
} // namespace yave::data