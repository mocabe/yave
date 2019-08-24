//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/lib/time/time.hpp>

namespace yave {

  /// Data struct for KeyframeFloat
  struct kf_float_data
  {
    /// Control point for KeyframeFloat
    struct control_point
    {
      /// relative time axis position
      time time;
      /// value
      double value;
    };

    /// lhs control point
    control_point cp_left;
    /// rhs control point
    control_point cp_right;
    /// value
    double value;
  };

} // namespace yave