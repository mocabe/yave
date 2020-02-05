//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/frame_time/frame_time.hpp>

namespace yave {

  namespace node {
    /// Time transform
    struct TimeTransform;
  } // namespace node

  template <>
  struct node_declaration_traits<node::TimeTransform>
  {
    static auto get_node_declaration() -> node_declaration
    {
      class X;
      return node_declaration(
        "TimeTransform",
        {"target", "time"},
        {"out"},
        "Transform time of subtree");
    }
  };
} // namespace yave
