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

  template <>
  struct node_declaration_traits<node::LayerCompositor>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "LayerCompositor",
        {"src", "dst", "blend op"},
        {"out"},
        {object_type<node_closure<
          FrameBuffer,
          FrameBuffer,
          node_closure<FrameBuffer, FrameBuffer, FrameBuffer>,
          FrameBuffer>>()},
        node_type::normal);
    }
  };

} // namespace yave