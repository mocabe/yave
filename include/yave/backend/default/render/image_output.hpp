//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/obj/image_output.hpp>

#include <yave/data/obj/frame_buffer.hpp>
#include <yave/node/obj/function.hpp>
#include <yave/node/obj/instance_getter.hpp>
#include <yave/backend/default/system/config.hpp>

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
  struct bind_info_traits<LayerImageOutput, backend::tags::default_render>
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