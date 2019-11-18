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
    /// Layer compositor node
    struct LayerCompositor;
  } // namespace node

  // info traits
  template <>
  struct node_info_traits<node::LayerCompositor>
  {
    static auto get_node_info() -> node_info
    {
      return node_info("LayerCompositor", {"src", "dst", "blend op"}, {"out"});
    }

    static auto get_node_type() -> object_ptr<const Type>
    {
      return object_type<node_closure<
        FrameBuffer,
        FrameBuffer,
        node_closure<FrameBuffer, FrameBuffer, FrameBuffer>,
        FrameBuffer>>();
    }
  };

} // namespace yave