//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/core/config.hpp>
#include <yave/module/std/core/decl/time_transform.hpp>
#include <yave/node/core/instance_getter.hpp>

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
        auto time = eval(arg<1>() << arg<2>());
        return arg<0>() << make_object<FrameDemand>(*time);
      }
    };

  } // namespace modules::_std::core

  template <>
  struct node_definition_traits<node::TimeTransform, modules::_std::core::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::TimeTransform>();
      return {node_definition(
        info.name(),
        info.output_sockets()[0],
        make_object<
          InstanceGetterFunction<modules::_std::core::TimeTransform>>(),
        info.name())};
    }
  };
} // namespace yave