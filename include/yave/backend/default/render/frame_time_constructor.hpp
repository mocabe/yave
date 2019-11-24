//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/default/config.hpp>
#include <yave/node/class/frame_time.hpp>
#include <yave/node/core/instance_getter.hpp>

namespace yave {

  namespace backend::default_render {

    /// FrameDemand -> FrameTime
    struct FrameTimeConstructor
      : Function<FrameTimeConstructor, FrameDemand, FrameTime>
    {
      return_type code() const
      {
        return make_object<FrameTime>(eval_arg<0>()->time);
      }
    };

  } // namespace backend::default_render

  template <>
  struct node_definition_traits<node::FrameTime, backend::tags::default_render>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::FrameTime>();
      return {node_definition(
        info.name(),
        info.output_sockets()[0],
        make_object<InstanceGetterFunction<
          backend::default_render::FrameTimeConstructor>>(),
        info.name())};
    }
  };

} // namespace yave