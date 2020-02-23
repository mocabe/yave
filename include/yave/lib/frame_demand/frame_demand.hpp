//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/frame_time/frame_time.hpp>
#include <yave/lib/vec/vec.hpp>
#include <yave/lib/mat/mat.hpp>

namespace yave {

  /// frame demand
  struct frame_demand
  {
    /// Current time.
    /// Initial value will be given as current render time.
    frame_time time;
    /// Current transformation matrix.
    /// Transformation is done by multiplying new matrix from right side.
    /// Initial matrix will be given as E.
    fmat4 transform;
  };
}