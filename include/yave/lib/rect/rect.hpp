//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/vec/vec.hpp>

namespace yave {

  /// 2D rectangle
  struct rect2
  {
    fvec2 offset;
    fvec2 extent;
  };

  /// 3D rectangle
  struct rect3
  {
    fvec3 offset;
    fvec3 extent;
  };
}