//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>

namespace yave {

  namespace node {
    /// SetTransform
    struct SetTransform;
  } // namespace node

  template <>
  struct node_declaration_traits<node::SetTransform>
  {
    static auto get_node_declaration()
    {
      return node_declaration(
        "SetTransform",
        "Apply new transformation to subtree",
        "std::transform",
        {"target", "transform"},
        {"out"});
    }
  };
}