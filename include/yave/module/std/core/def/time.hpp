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

    /// FrameDemand -> FrameTime
    struct TimeConstructor
      : NodeFunction<TimeConstructor, FrameDemand, FrameTime>
    {
      return_type code() const
      {
        return make_object<FrameTime>(eval_arg<0>()->time);
      }
    };

    /// FrameTime -> FrameTime
    struct TimeConstructor2
      : NodeFunction<TimeConstructor2, FrameTime, FrameTime>
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

      auto def1 = node_definition(
        info.name(),
        0,
        make_object<
          InstanceGetterFunction<modules::_std::core::TimeConstructor>>(),
        "Internal: Construct Time from FrameDemand object");

      auto def2 = node_definition(
        info.name(),
        0,
        make_object<
          InstanceGetterFunction<modules::_std::core::TimeConstructor2>>(),
        "Construct Time fromt Time");

      return {std::move(def1), std::move(def2)};
    }
  };

} // namespace yave