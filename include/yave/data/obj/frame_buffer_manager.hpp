//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/rts/box.hpp>
#include <yave/data/lib/frame_buffer_manager.hpp>

namespace yave {

  /// Frame buffer object
  using FrameBufferManager = Box<frame_buffer_manager>;

} // namespace yave

YAVE_DECL_TYPE(yave::FrameBufferManager, "99d60d8c-01bd-4628-b342-b6516ac4ae1d");