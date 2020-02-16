//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/frame_time/frame_time.hpp>
#include <yave/lib/vec/vec.hpp>

namespace yave {

  /// frame demand
  struct frame_demand
  {
    /// time
    frame_time time;
    /// position
    fvec3 position;
  };
}