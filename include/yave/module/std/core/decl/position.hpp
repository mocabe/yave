//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/rts/identity.hpp>

namespace yave {

  namespace node {
    /// position
    struct Position;
  } // namespace node

  template <>
  struct node_declaration_traits<node::Position>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "Position",
        {"value"},
        {"value"},
        "Constructor of pos value. This node construct new Vec3 object from "
        "argument, or creates object which represents current position applied "
        "to the tree.",
        {{0, make_object<Identity>()}});
    }
  };
} // namespace yave