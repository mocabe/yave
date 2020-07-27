//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/primitive/conversion.hpp>
#include <yave/node/core/function.hpp>

namespace yave {

  auto node_declaration_traits<node::IntToFloat>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "IntToFloat", "/std/prim", "Convert Int to Float", {"int"}, {"float"});
  }

  namespace modules::_std::prim {

    struct IntToFloat : NodeFunction<IntToFloat, Int, Float>
    {
      auto code() const -> return_type
      {
        return make_object<Float>(static_cast<float>(*eval_arg<0>()));
      }
    };
  } // namespace modules::_std::prim

  auto node_definition_traits<node::IntToFloat, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::IntToFloat>();
    return std::vector {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::prim::IntToFloat>(),
      info.description())};
  }

} // namespace yave