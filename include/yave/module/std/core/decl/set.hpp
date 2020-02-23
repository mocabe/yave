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
    /// SetTime
    struct SetTime;
  } // namespace node

  template <>
  struct node_declaration_traits<node::SetTime>
  {
    static auto get_node_declaration()
    {
      return node_declaration(
        "SetTime", {"target", "time"}, {"out"}, "Apply new time to subtree");
    }
  };

  template <>
  struct node_declaration_traits<node::SetTransform>
  {
    static auto get_node_declaration()
    {
      return node_declaration(
        "SetTransform",
        {"target", "transform"},
        {"out"},
        "Apply new transformation to subtree");
    }
  };
}