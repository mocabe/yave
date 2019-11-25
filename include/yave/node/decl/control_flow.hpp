//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/primitive/primitive.hpp>

namespace yave {

  namespace node {
    /// If
    struct If;
  } // namespace node

  template <>
  struct node_declaration_traits<node::If>
  {
    static auto get_node_declaration() -> node_declaration
    {
      class X;
      return node_declaration(
        "If",
        {"cond", "then", "else"},
        {"out"},
        {object_type<node_closure<Bool, X, X, X>>()},
        node_type::normal);
    }
  };

  inline auto get_control_flow_node_declaration_list()
    -> std::vector<node_declaration>
  {
    return {get_node_declaration<node::If>()};
  }
}