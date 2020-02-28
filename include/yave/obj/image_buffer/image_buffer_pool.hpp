//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/lib/image_buffer/image_buffer_pool.hpp>

namespace yave {

  /// Image buffer pool object
  using ImageBufferPool = Box<image_buffer_pool>;

} // namespace yave

YAVE_DECL_TYPE(yave::ImageBufferPool, "68903588-a638-4b6b-b025-ee2a5bc79a01");