//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/core/decl/primitive.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>

namespace yave {

  namespace modules::_std::core {

    template <class T>
    struct DataTypeConstructor : NodeFunction<DataTypeConstructor<T>, T, T>
    {
      auto code() const -> typename DataTypeConstructor::return_type
      {
        return DataTypeConstructor::template eval_arg<0>();
      }
    };

  } // namespace modules::_std::core

  template <class T>
  struct node_definition_traits<
    node::DataTypeConstructor<T>,
    modules::_std::core::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::DataTypeConstructor<T>>();

      return {node_definition(
        info.name(),
        0,
        make_object<modules::_std::core::DataTypeConstructor<T>>(),
        info.name())};
    }
  };

} // namespace yave