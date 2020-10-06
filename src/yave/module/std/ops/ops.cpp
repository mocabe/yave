//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/ops/ops.hpp>
#include <yave/node/core/function.hpp>
#include <yave/node/core/generator.hpp>

namespace yave {

  auto node_declaration_traits<node::Add>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Add", "/std/ops", "std/ops/Add", {"x", "y"}, {"x+y"});
  }

  auto node_declaration_traits<node::Sub>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Sub", "/std/ops", "std/ops/Sub", {"x", "y"}, {"x-y"});
  }

  auto node_declaration_traits<node::Mul>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Mul", "/std/ops", "std/ops/Mul", {"x", "y"}, {"x*y"});
  }

  auto node_declaration_traits<node::Div>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Div", "/std/ops", "std/ops/Div", {"x", "y"}, {"x/y"});
  }
} // namespace yave