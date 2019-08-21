//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/time/time.hpp>

namespace yave {

  /// Data type to represent single frame point.
  struct frame
  {
    /// Time point of frame
    yave::time time;
    /// Lendth of current frame
    yave::time length;
  };

} // namespace yave