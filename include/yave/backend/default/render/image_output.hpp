//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/default/config.hpp>
#include <yave/node/obj/image_output.hpp>
#include <yave//obj/frame_buffer/frame_buffer.hpp>
#include <yave/node/core/function.hpp>
#include <yave/node/core/instance_getter.hpp>

namespace yave {

  namespace backend::default_render {

    /// FrameBuffer -> FrameBuffer
    struct LayerImageOutput
      : NodeFunction<LayerImageOutput, FrameBuffer, FrameBuffer>
    {
      return_type code() const
      {
        return arg<0>();
      }
    };

  } // namespace backend::default_render

  template <>
  struct bind_info_traits<node::LayerImageOutput, backend::tags::default_render>
  {
    static bind_info get_bind_info()
    {
      auto info = get_node_info<node::LayerImageOutput>();
      return bind_info(
        info.name(),
        info.input_sockets(),
        info.output_sockets()[0],
        make_object<
          InstanceGetterFunction<backend::default_render::LayerImageOutput>>(),
        info.name() + ": Image output node for layers");
    }
  };
}