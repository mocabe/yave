//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/core/decl/primitive.hpp>
#include <yave/obj/behaviour/frame_demand.hpp>

namespace yave {

  namespace modules::_std::core {

    template <class T>
    struct PrimitiveConstructor : NodeFunction<PrimitiveConstructor<T>, T, T>
    {
      auto code() const -> typename PrimitiveConstructor::return_type
      {
        return PrimitiveConstructor::template eval_arg<0>();
      }
    };

  } // namespace modules::_std::core

  template <class T>
  struct node_definition_traits<
    node::PrimitiveConstructor<T>,
    modules::_std::core::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::PrimitiveConstructor<T>>();

      return {node_definition(
        info.name(),
        info.output_sockets()[0],
        make_object<InstanceGetterFunction<
          modules::_std::core::PrimitiveConstructor<T>>>(),
        info.name())};
    }
  };

} // namespace yave