//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/frame_time/frame_time.hpp>

namespace yave {

  /// frame demand
  struct frame_demand
  {
    /// Current time.
    /// Initial value will be given as current render time.
    frame_time time;
  };
}