//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/frame_buffer/frame_buffer.hpp>

namespace yave {

  namespace node {
    /// Frame buffer constructor
    struct FrameBuffer;
  } // namespace node

  template <>
  struct node_info_traits<node::FrameBuffer>
  {
    static auto get_node_info() -> node_info
    {
      // pseudo primitive
      return node_info("FrameBuffer", {}, {"value"}, node_type::primitive);
    }

    static auto get_node_type() -> object_ptr<const Type>
    {
      return object_type<node_closure<FrameBuffer>>();
    }
  };
} // namespace yave