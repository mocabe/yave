//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>

namespace yave {

  namespace node {
    /// Frame buffer constructor
    struct FrameBuffer;
  } // namespace node

  template <>
  struct node_info_traits<node::FrameBuffer>
  {
    static node_info get_node_info()
    {
      // pseudo primitive
      return node_info("FrameBuffer", {}, {"value"}, node_type::primitive);
    }
  };
} // namespace yave