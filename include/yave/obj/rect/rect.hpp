//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/rect/rect.hpp>
#include <yave/rts/box.hpp>

namespace yave {
  /// 2D Rect object
  using Rect2 = Box<rect2>;
  /// 3D Rect object
  using Rect3 = Box<rect3>;
} // namespace yave

YAVE_DECL_TYPE(yave::Rect2, "e0a60a33-1fca-4243-8c79-0816236061f5");
YAVE_DECL_TYPE(yave::Rect3, "e1083566-20fa-4273-ba03-2cb4bf23c7b5");