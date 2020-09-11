//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/data/frame_time/frame_time.hpp>

namespace yave {

  /// FrameTime object
  using FrameTime = Box<data::frame_time>;

} // namespace yave

YAVE_DECL_TYPE(yave::FrameTime, "510a0f3a-8c18-46f9-9cc6-514609e35c13");