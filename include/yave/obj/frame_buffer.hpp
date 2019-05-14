//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/frame_buffer/frame_buffer.hpp>
#include <yave/rts/box.hpp>

namespace yave {
  /// namespace for FrameBuffer object and functions
  namespace FrameBuffer {
    /// Frame buffer object
    using FrameBuffer = Box<frame_buffer>;
  } // namespace FrameBuffer
} // namespace yave

YAVE_DECL_TYPE(
  yave::FrameBuffer::FrameBuffer,
  "cfad3b2d-4979-4cae-b63b-30be9b8d60eb");