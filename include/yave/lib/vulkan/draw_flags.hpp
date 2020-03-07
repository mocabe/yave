//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/support/enum_flag.hpp>

namespace yave::vulkan {

  /// polyline options
  enum class polyline_flags
  {
    /// closed
    closed = 1 << 0,
  };

  /// polygon options
  enum class polygon_flags
  {
    // reserved
  };
}

YAVE_DECL_ENUM_FLAG(yave::vulkan::polyline_flags);
YAVE_DECL_ENUM_FLAG(yave::vulkan::polygon_flags);