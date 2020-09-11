//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/data/buffer/buffer_pool.hpp>

namespace yave {

  /// General buffer object
  using BufferPool = Box<data::buffer_pool>;

} // namespace yave

YAVE_DECL_TYPE(yave::BufferPool, "e3d0d9fe-7af2-4759-a109-e585d3603f03");