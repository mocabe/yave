//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/time/time.hpp>
#include <yave/rts/box.hpp>

namespace yave {
  /// Namespace for Time object and functions.
  namespace Time {
    /// Time object
    using Time = Box<time>;
  } // namespace Time
} // namespace yave

YAVE_DECL_TYPE(yave::Time::Time, _Time);