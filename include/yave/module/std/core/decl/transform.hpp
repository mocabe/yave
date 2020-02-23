//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/rts/identity.hpp>

namespace yave {

  namespace node {
    /// Transform
    struct Transform;
  } // namespace node

  template <>
  struct node_declaration_traits<node::Transform>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "Transform",
        {"value"},
        {"value"},
        "Make or get current transform matrix",
        {{0, make_object<Identity>()}});
    }
  };
} // namespace yave