//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/default/config.hpp>
#include <yave/node/class/compositor.hpp>
#include <yave/node/core/instance_getter.hpp>

namespace yave {

  namespace backend::default_render {

    /// Compositor
    struct LayerCompositor
      : NodeFunction<
          LayerCompositor,
          FrameBuffer,                                    // src
          FrameBuffer,                                    // dst
          closure<FrameBuffer, FrameBuffer, FrameBuffer>, // blend op
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
  struct node_definition_traits<
    node::LayerCompositor,
    backend::tags::default_render>
  {
    static auto get_node_definition() -> node_definition
    {
      auto info = get_node_declaration<node::LayerCompositor>();
      return node_definition(
        info.name(),
        info.output_sockets()[0],
        info.name() + ": composite layer outputs",
        make_object<
          InstanceGetterFunction<backend::default_render::LayerCompositor>>());
    }
  };

} // namespace yave