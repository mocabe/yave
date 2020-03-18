//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/primitive/primitive.hpp>
#include <yave/module/std/primitive/data_holder.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>

namespace yave {

  template <class T>
  auto node_declaration_traits<
    node::DataTypeConstructor<T>>::get_node_declaration() -> node_declaration
  {
    return node_declaration(
      data_type_traits<T>::name,
      "Data type constructor",
      "std::prim",
      {"value"},
      {"value"},
      {{0, make_data_type_holder<T>()}});
  }

  namespace modules::_std::prim {

    template <class T>
    struct DataTypeConstructor : NodeFunction<DataTypeConstructor<T>, T, T>
    {
      auto code() const -> typename DataTypeConstructor::return_type
      {
        return DataTypeConstructor::template eval_arg<0>();
      }
    };

  } // namespace modules::_std::prim

  template <class T>
  auto node_definition_traits<
    node::DataTypeConstructor<T>,
    modules::_std::tag>::get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::DataTypeConstructor<T>>();

    return {node_definition(
      info.name(),
      0,
      make_object<modules::_std::prim::DataTypeConstructor<T>>(),
      info.name())};
  }

} // namespace yave