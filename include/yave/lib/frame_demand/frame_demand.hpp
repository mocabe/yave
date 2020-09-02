//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/obj/frame_time/frame_time.hpp>

namespace yave {

  /// frame demand
  struct frame_demand
  {
    /// time
    object_ptr<const FrameTime> time;
  };
}