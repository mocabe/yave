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
    /// Layer image output
    struct LayerImageOutput;
  } // namespace node

  template <>
  struct node_info_traits<node::LayerImageOutput>
  {
    static auto get_node_info() -> node_info
    {
      return node_info("LayerImageOutput", {"in"}, {"out"});
    }

    static auto get_node_type() -> object_ptr<const Type>
    {
      class X;
      return object_type<node_closure<X, FrameBuffer>>();
    }
  };
}