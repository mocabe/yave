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

  inline bool operator==(
    const kf_float_data::control_point& lhs,
    const kf_float_data::control_point& rhs)
  {
    return lhs.time == rhs.time && lhs.value == rhs.value;
  }

  inline bool operator!=(
    const kf_float_data::control_point& lhs,
    const kf_float_data::control_point& rhs)
  {
    return !(lhs == rhs);
  }

  inline bool operator==(const kf_float_data& lhs, const kf_float_data& rhs)
  {
    return lhs.cp_left == rhs.cp_left && lhs.cp_right == rhs.cp_right &&
           lhs.value == rhs.value;
  }

  inline bool operator!=(const kf_float_data& lhs, const kf_float_data& rhs)
  {
    return !(lhs == rhs);
  }

} // namespace yave