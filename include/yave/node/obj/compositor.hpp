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

  struct LayerBlendOp
    : NodeFunction<LayerBlendOp, FrameBuffer, FrameBuffer, FrameBuffer>
  {
    // just for type
  };

  /// Compositor
  struct LayerCompositor : NodeFunction<
                             LayerCompositor,
                             FrameBuffer,  // src
                             FrameBuffer,  // dst
                             LayerBlendOp, // blend op
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
      return node_info("LayerCompositor", {"src", "dst", "blend op"}, {"out"});
    }
    static bind_info get_bind_info()
    {
      return bind_info(
        "LayerCompositor",
        {"src", "dst", "blend op"},
        "out",
        make_object<InstanceGetterFunction<LayerCompositor>>(),
        "LayerCompositor");
    }
  };

} // namespace yave