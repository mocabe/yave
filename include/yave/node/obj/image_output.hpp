//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/data/obj/frame_buffer.hpp>
#include <yave/node/obj/function.hpp>
#include <yave/node/obj/instance_getter.hpp>
#include <yave/node/core/get_info.hpp>

namespace yave {

  /// FrameBuffer -> FrameBuffer
  struct LayerImageOutput
    : NodeFunction<LayerImageOutput, FrameBuffer, FrameBuffer>
  {
    return_type code() const
    {
      return arg<0>();
    }
  };

  // info traits
  template <>
  struct node_function_info_traits<LayerImageOutput>
  {
    static node_info get_node_info()
    {
      return node_info("LayerImageOutput", {"in"}, {"out"});
    }

    static bind_info get_bind_info()
    {
      return bind_info(
        "LayerImageOutput",
        {"in"},
        "out",
        make_object<InstanceGetterFunction<LayerImageOutput>>(),
        "Image output node for layers.");
    }
  };
}