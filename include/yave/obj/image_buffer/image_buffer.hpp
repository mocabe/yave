//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/lib/image_buffer/image_buffer.hpp>

namespace yave {

  /// Image buffer
  using ImageBuffer = Box<image_buffer>;

} // namespace yave

YAVE_DECL_TYPE(yave::ImageBuffer, "e8285215-660c-4654-a0e8-c3f8abdb1487");