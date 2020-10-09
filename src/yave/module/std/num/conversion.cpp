//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/num/conversion.hpp>
#include <yave/node/core/function.hpp>

namespace yave {

  auto node_declaration_traits<node::ToFloat>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Std.Num.ToFloat", "Convert Num to Float", {"num"}, {"float"});
  }

  auto node_declaration_traits<node::ToInt>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Std.Num.ToInt", "Convert Num to Int", {"num"}, {"int"});
  }

  namespace modules::_std::prim {

    struct IntToFloat : NodeFunction<IntToFloat, Int, Float>
    {
      auto code() const -> return_type
      {
        return make_object<Float>(static_cast<double>(*eval_arg<0>()));
      }
    };

    struct FloatToInt : NodeFunction<FloatToInt, Float, Int>
    {
      auto code() const -> return_type
      {
        return make_object<Int>(static_cast<long>(*eval_arg<0>()));
      }
    };

  } // namespace modules::_std::prim

  auto node_definition_traits<node::ToFloat, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::ToFloat>();
    return std::vector {node_definition(
      info.full_name(), 0, make_object<modules::_std::prim::IntToFloat>())};
  }

  auto node_definition_traits<node::ToInt, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::ToInt>();
    return std::vector {node_definition(
      info.full_name(), 0, make_object<modules::_std::prim::FloatToInt>())};
  }

} // namespace yave