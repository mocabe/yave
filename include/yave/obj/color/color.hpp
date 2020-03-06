//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/color/color.hpp>
#include <yave/rts/box.hpp>

namespace yave {
  /// Color vlaue object
  using Color = Box<color>;
} // namespace yave

YAVE_DECL_TYPE(yave::Color, "3713e9f5-05c2-4022-8f62-16ca0f14518f");