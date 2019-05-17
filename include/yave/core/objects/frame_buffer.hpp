//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/core/data_types/frame_buffer.hpp>
#include <yave/core/rts/box.hpp>

namespace yave {

  /// Frame buffer object
  using FrameBuffer = Box<frame_buffer>;

} // namespace yave

YAVE_DECL_TYPE(yave::FrameBuffer, "cfad3b2d-4979-4cae-b63b-30be9b8d60eb");