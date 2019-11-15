//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <imgui.h>

namespace yave::imgui {

  inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
  {
    return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
  }

  inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)
  {
    return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y);
  }

  inline ImVec2 operator*(const ImVec2& vec, float scalar)
  {
    return ImVec2(vec.x * scalar, vec.y * scalar);
  }

  inline ImVec2 operator/(const ImVec2& vec, float scalar)
  {
    return ImVec2(vec.x / scalar, vec.y / scalar);
  }

  inline bool operator==(const ImVec2& lhs, const ImVec2& rhs)
  {
    return lhs.x == rhs.x && lhs.y == rhs.y;
  }

  inline bool operator!=(const ImVec2& lhs, const ImVec2& rhs)
  {
    return !(lhs == rhs);
  }
} // namespace yave