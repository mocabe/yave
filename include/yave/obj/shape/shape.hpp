//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/lib/shape/shape.hpp>

namespace yave {
  /// Shape
  /// FIXME: Use ABI stable data type
  using Shape = Box<shape>;
} // namespace yave

YAVE_DECL_TYPE(yave::Shape, "4aeb6c88-2315-4eba-a45f-fcd54982588d");