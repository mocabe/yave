//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/lib/time/time.hpp>

namespace yave {
  /// Time object
  using Time = Box<time>;
} // namespace yave

YAVE_DECL_TYPE(yave::Time, "074d6cdc-82dc-4f42-99d7-5e556543038e");