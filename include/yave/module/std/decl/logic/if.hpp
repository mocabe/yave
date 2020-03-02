//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>

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
        "Basic control primitive. Takes single (Bool) value to switch call to "
        "subgraph. Because of lazy evaluation, subgraph not taken will not be "
        "evaluated",
        "std::logic",
        {"cond", "then", "else"},
        {"out"});
    }
  };

} // namespace yave