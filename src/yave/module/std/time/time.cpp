//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/time/time.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/obj/frame_time/frame_time.hpp>

namespace yave {

  auto node_declaration_traits<node::Time>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Time",
      "std/time",
      "Constructor of time value. This node construct new FrameTime object "
      "from argument, or creates FrameTime object which represents current "
      "time applied to the tree.",
      {"value"},
      {"value"},
      {{0, make_object<modules::_std::core::GetCurrentTime>()}});
  }

  namespace modules::_std::time {
    /// FrameTime -> FrameTime
    struct TimeConstructor : NodeFunction<TimeConstructor, FrameTime, FrameTime>
    {
      return_type code() const
      {
        return arg<0>();
      }
    };
  } // namespace modules::_std::time

  auto node_definition_traits<node::Time, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Time>();

    return {node_definition(
      info.name(),
      0,
      make_object<modules::_std::time::TimeConstructor>(),
      "Construct Time fromt Time")};
  }

} // namespace yave