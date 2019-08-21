//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/lib/frame_buffer/frame_buffer_pool.hpp>

namespace yave {

  /// Frame buffer object
  using FrameBufferPool = Box<frame_buffer_pool>;

} // namespace yave

YAVE_DECL_TYPE(yave::FrameBufferPool, "99d60d8c-01bd-4628-b342-b6516ac4ae1d");