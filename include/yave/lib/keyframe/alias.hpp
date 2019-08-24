//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/keyframe/keyframe.hpp>
#include <yave/lib/keyframe/float.hpp>

namespace yave {

  /* keyframe typedefs */

  /// keyframe for integer value
  using kf_int_t = keyframe<int64_t>;
  /// keyframe for floating point values (including interpolation handles)
  using kf_float_t = keyframe<kf_float_data>;
  /// keyframe for boolean values
  using kf_bool_t = keyframe<bool>;

} // namespace yave