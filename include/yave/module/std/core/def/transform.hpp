//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/core/config.hpp>
#include <yave/module/std/core/decl/transform.hpp>
#include <yave/node/core/instance_getter.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/frame_time/frame_time.hpp>
#include <yave/obj/vec/vec.hpp>

namespace yave {

  namespace modules::_std::core {

    class X;
    /// X -> FrameTime -> X
    struct TimeTransform : Function<
                             TimeTransform,
                             node_closure<forall<X>>,
                             node_closure<FrameTime>,
                             FrameDemand,
                             forall<X>>
    {
      return_type code() const
      {
        auto demand = eval_arg<2>();
        auto time   = eval(arg<1>() << demand);
        return arg<0>() << make_object<FrameDemand>(*time, demand->position);
      }
    };

    class Y;
    /// Y -> Vec3 -> Y
    struct PositionTransform : Function<
                                 PositionTransform,
                                 node_closure<forall<Y>>,
                                 node_closure<FVec3>,
                                 FrameDemand,
                                 forall<Y>>
    {
      return_type code() const
      {
        auto demand = eval_arg<2>();
        auto pos    = eval(arg<1>() << demand);
        return arg<0>() << make_object<FrameDemand>(demand->time, *pos);
      }
    };

  } // namespace modules::_std::core

  template <>
  struct node_definition_traits<node::Transform, modules::_std::core::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::Transform>();

      auto d1 = node_definition(
        info.name(),
        0,
        make_object<
          InstanceGetterFunction<modules::_std::core::TimeTransform>>(),
        info.name());

      auto d2 = node_definition(
        info.name(),
        0,
        make_object<
          InstanceGetterFunction<modules::_std::core::PositionTransform>>(),
        info.name());

      return {std::move(d1), std::move(d2)};
    }
  };
} // namespace yave