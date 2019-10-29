//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/time/time.hpp>

namespace yave {

  /// Data type to represent frame time to render.
  struct frame_time
  {
    /// Time point of frame.
    yave::time time_point;
    /// Duration of frame to render.
    yave::time duration;
  };

} // namespace yave