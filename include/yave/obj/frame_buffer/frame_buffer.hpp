//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/data/frame_buffer/frame_buffer.hpp>

namespace yave {

  /// Frame buffer object
  using FrameBuffer = Box<data::frame_buffer>;

} // namespace yave

YAVE_DECL_TYPE(yave::FrameBuffer, "cfad3b2d-4979-4cae-b63b-30be9b8d60eb");