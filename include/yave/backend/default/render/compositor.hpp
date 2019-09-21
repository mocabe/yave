//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/default/config.hpp>
#include <yave/node/obj/compositor.hpp>
#include <yave/node/obj/function.hpp>
#include <yave/data/obj/frame_buffer.hpp>
#include <yave/node/obj/instance_getter.hpp>

namespace yave {

  namespace backend::default_render {

    /// Compositor
    struct LayerCompositor
      : NodeFunction<
          LayerCompositor,
          FrameBuffer,                                         // src
          FrameBuffer,                                         // dst
          node_closure<FrameBuffer, FrameBuffer, FrameBuffer>, // blend op
          FrameBuffer>
    {
      return_type code() const
      {
        // apply blend function
        return arg<2>() << arg<0>() << arg<1>();
      }
    };

  } // namespace backend::default_render

  template <>
  struct bind_info_traits<node::LayerCompositor, backend::tags::default_render>
  {
    static bind_info get_bind_info()
    {
      auto info = get_node_info<node::LayerCompositor>();
      return bind_info(
        info.name(),
        info.input_sockets(),
        info.output_sockets()[0],
        make_object<
          InstanceGetterFunction<backend::default_render::LayerCompositor>>(),
        info.name() + ": composite layer outputs");
    }
  };

} // namespace yave