//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/core/config.hpp>
#include <yave/module/std/core/decl/time.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/obj/frame_time/frame_time.hpp>
#include <yave/node/core/instance_getter.hpp>

namespace yave {

  namespace modules::_std::core {
    /// FrameTime -> FrameTime
    struct TimeConstructor : NodeFunction<TimeConstructor, FrameTime, FrameTime>
    {
      return_type code() const
      {
        return arg<0>();
      }
    };
  } // namespace modules::_std::core

  template <>
  struct node_definition_traits<node::Time, modules::_std::core::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::Time>();

      return {node_definition(
        info.name(),
        0,
        make_object<
          InstanceGetterFunction<modules::_std::core::TimeConstructor>>(),
        "Construct Time fromt Time")};
    }
  };

} // namespace yave