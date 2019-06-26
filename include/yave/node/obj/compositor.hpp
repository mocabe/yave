//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/obj/function.hpp>
#include <yave/data/obj/frame_buffer.hpp>
#include <yave/node/obj/instance_getter.hpp>
#include <yave/node/core/get_info.hpp>

namespace yave {

  /// Compositor
  struct LayerCompositor
    : NodeFunction<
        FrameBuffer,                                    // src
        FrameBuffer,                                    // dst
        closure<FrameBuffer, FrameBuffer, FrameBuffer>, // blend func
        FrameBuffer>
  {
    return_type code() const
    {
      // apply blend function
      return arg<2>() << arg<0>() << arg<1>();
    }
  };

  // info traits
  template <>
  struct node_function_info_traits<LayerCompositor>
  {
    static node_info get_node_info()
    {
      return node_info(
        "LayerImageOutput", {"src", "dst", "blend_func"}, {"out"});
    }
    static bind_info get_bind_info()
    {
      return bind_info(
        "LayerCompositor",
        {"src", "dst", "blend func"},
        "out",
        make_object<InstanceGetterFunction<LayerCompositor>>(),
        "Layer composition node.");
    }
  };

} // namespace yave