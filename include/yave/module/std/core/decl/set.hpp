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
        "SetTime",
        "Apply new time to subtree",
        "std::core::reactive",
        {"target", "time"},
        {"out"});
    }
  };

  template <>
  struct node_declaration_traits<node::SetTransform>
  {
    static auto get_node_declaration()
    {
      return node_declaration(
        "SetTransform",
        "Apply new transformation to subtree",
        "std::core::reactive",
        {"target", "transform"},
        {"out"});
    }
  };
}