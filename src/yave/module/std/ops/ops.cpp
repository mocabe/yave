//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/ops/ops.hpp>
#include <yave/node/core/function.hpp>
#include <yave/node/core/generator.hpp>

namespace yave {

  auto node_declaration_traits<node::Ops::Add>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Ops.Add", "", node_declaration_visibility::_public, {"x", "y"}, {"x+y"});
  }

  auto node_declaration_traits<node::Ops::Sub>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Ops.Sub", "", node_declaration_visibility::_public, {"x", "y"}, {"x-y"});
  }

  auto node_declaration_traits<node::Ops::Mul>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Ops.Mul", "", node_declaration_visibility::_public, {"x", "y"}, {"x*y"});
  }

  auto node_declaration_traits<node::Ops::Div>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Ops.Div", "", node_declaration_visibility::_public, {"x", "y"}, {"x/y"});
  }
} // namespace yave