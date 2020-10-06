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
    return node_declaration("Std.Ops.Add", "", {"x", "y"}, {"x+y"});
  }

  auto node_declaration_traits<node::Sub>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration("Std.Ops.Sub", "", {"x", "y"}, {"x-y"});
  }

  auto node_declaration_traits<node::Mul>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration("Std.Ops.Mul", "", {"x", "y"}, {"x*y"});
  }

  auto node_declaration_traits<node::Div>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration("Std.Ops.Div", "", {"x", "y"}, {"x/y"});
  }
} // namespace yave