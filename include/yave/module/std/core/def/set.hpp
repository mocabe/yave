
#pragma once

#include <yave/module/std/core/config.hpp>
#include <yave/module/std/core/decl/set.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/obj/frame_time/frame_time.hpp>
#include <yave/obj/mat/mat.hpp>

namespace yave {

  namespace modules::_std::core {

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
        return arg<0>() << make_object<FrameDemand>(*t, fd->transform);
      }
    };

    class SetTransform_X;

    struct SetTransform : Function<
                            SetTransform,
                            node_closure<forall<SetTransform_X>>,
                            node_closure<FMat4>,
                            FrameDemand,
                            forall<SetTransform_X>>
    {
      return_type code() const
      {
        auto fd = eval_arg<2>();
        auto m  = eval(arg<1>() << fd);
        return arg<0>() << make_object<FrameDemand>(fd->time, *m);
      }
    };
  }

  template <>
  struct node_definition_traits<node::SetTime, modules::_std::core::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::SetTime>();

      return {node_definition(
        info.name(),
        0,
        make_object<InstanceGetterFunction<modules::_std::core::SetTime>>(),
        info.description())};
    }
  };

  template <>
  struct node_definition_traits<node::SetTransform, modules::_std::core::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::SetTransform>();

      return {node_definition(
        info.name(),
        0,
        make_object<
          InstanceGetterFunction<modules::_std::core::SetTransform>>(),
        info.description())};
    }
  };
} // namespace yave