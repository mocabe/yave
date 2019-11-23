//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/default/config.hpp>
#include <yave/node/class/image_output.hpp>
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
  struct node_definition_traits<
    node::LayerImageOutput,
    backend::tags::default_render>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::LayerImageOutput>();
      return {node_definition(
        info.name(),
        info.output_sockets()[0],
        make_object<
          InstanceGetterFunction<backend::default_render::LayerImageOutput>>(),
        info.name() + ": Image output node for layers")};
    }
  };
}