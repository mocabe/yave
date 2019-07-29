//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>

namespace yave {

  /// Frame buffer constructor
  struct FrameBufferConstructor;

  template <>
  struct node_info_traits<FrameBufferConstructor>
  {
    static node_info get_node_info()
    {
      return node_info("FrameBuffer", {}, {"value"});
    }
  };
} // namespace yave