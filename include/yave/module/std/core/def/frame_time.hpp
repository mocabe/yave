//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/core/config.hpp>
#include <yave/module/std/core/decl/frame_time.hpp>
#include <yave/obj/behaviour/frame_demand.hpp>
#include <yave/node/core/instance_getter.hpp>

namespace yave {

  namespace modules::_std::core {

    /// FrameDemand -> FrameTime
    struct FrameTimeConstructor
      : Function<FrameTimeConstructor, FrameDemand, FrameTime>
    {
      return_type code() const
      {
        return make_object<FrameTime>(eval_arg<0>()->time);
      }
    };

  } // namespace modules::_std::core

  template <>
  struct node_definition_traits<node::FrameTime, modules::_std::core::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::FrameTime>();
      return {node_definition(
        info.name(),
        info.output_sockets()[0],
        make_object<
          InstanceGetterFunction<modules::_std::core::FrameTimeConstructor>>(),
        info.name())};
    }
  };

} // namespace yave