//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/lib/buffer/buffer.hpp>

namespace yave {

  /// General buffer object
  using Buffer = Box<buffer>;

} // namespace yave

YAVE_DECL_TYPE(yave::Buffer, "86a7add6-b020-420f-84f9-b18c6e8082f2");