//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

namespace yave {

  /// Color
  struct color
  {
    float r = 0;
    float g = 0;
    float b = 0;
    float a = 0;
  };

  static_assert(sizeof(color) == sizeof(float) * 4);
} // namespace yave