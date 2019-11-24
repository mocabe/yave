//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/lib/behaviour/frame_demand.hpp>

namespace yave {

  /// FrameDemand object
  /// TODO: Use unboxed tuple
  using FrameDemand = Box<frame_demand>;

} // namespace yave

YAVE_DECL_TYPE(yave::FrameDemand, "3708b0fd-56b6-4455-8139-23c9b79fda63");