//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/node/core/function.hpp>

namespace yave {

  namespace node {
    /// Id function
    struct Identity;
  } // namespace node

  template <>
  struct node_declaration_traits<node::Identity>
  {
    static auto get_node_declaration() -> node_declaration
    {
      class X;
      return node_declaration(
        "Identity",
        {"in"},
        {"out"},
        {object_type<node_closure<X, X>>()},
        node_type::normal);
    }
  };
} // namespace yave