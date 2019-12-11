//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/default/config.hpp>
#include <yave/node/decl/compositor.hpp>
#include <yave/node/core/instance_getter.hpp>

namespace yave {

  namespace backends::default_render {

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

  } // namespace backends::default_render

  template <>
  struct node_definition_traits<
    node::LayerCompositor,
    backend_tags::default_render>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::LayerCompositor>();
      return {node_definition(
        info.name(),
        info.output_sockets()[0],
        make_object<
          InstanceGetterFunction<backends::default_render::LayerCompositor>>(),
        info.name() + ": composite layer outputs")};
    }
  };

} // namespace yave