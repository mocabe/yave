//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>

namespace yave {

  namespace node {
    /// Demand transformation
    struct Transform;
  } // namespace node

  template <>
  struct node_declaration_traits<node::Transform>
  {
    static auto get_node_declaration() -> node_declaration
    {
      class X;
      return node_declaration(
        "Transform", {"target", "arg"}, {"out"}, "Transform subtree");
    }
  };
} // namespace yave
