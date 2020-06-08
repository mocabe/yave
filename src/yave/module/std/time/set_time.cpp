//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/time/set_time.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/obj/frame_time/frame_time.hpp>

namespace yave {

  auto node_declaration_traits<node::SetTime>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "SetTime",
      "/std/time",
      "Apply new time to subtree",
      {"target", "time"},
      {"out"});
  }

  namespace modules::_std::time {

    class SetTime_X;

    struct SetTime : Function<
                       SetTime,
                       node_closure<forall<SetTime_X>>,
                       node_closure<FrameTime>,
                       FrameDemand,
                       forall<SetTime_X>>
    {
      return_type code() const
      {
        auto fd = eval_arg<2>();
        auto t  = eval(arg<1>() << fd);
        return arg<0>() << make_object<FrameDemand>(*t);
      }
    };

  } // namespace modules::_std::time

  auto node_definition_traits<node::SetTime, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::SetTime>();

    return {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::time::SetTime>(),
      info.description())};
  }
} // namespace yave