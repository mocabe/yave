//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>

namespace yave {

  namespace node {
    /// Bind
    struct Bind;
  } // namespace node

  template <>
  struct node_declaration_traits<node::Bind>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "Bind",
        "Apply argument to function",
        "std::core::functional",
        {"fn", "arg"},
        {"fn"});
    }
  };
} // namespace yave
