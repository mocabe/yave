//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>

namespace yave {

  namespace node {
    /// Pesudo primitive node for frame time.
    struct FrameTime;
  } // namespace node

  template <>
  struct node_info_traits<node::FrameTime>
  {
    static node_info get_node_info()
    {
      // pseudo primitive
      return node_info("FrameTime", {}, {"value"}, node_type::primitive);
    }
  };
} // namespace yave