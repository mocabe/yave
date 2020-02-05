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
    /// Pesudo primitive node for frame time.
    struct FrameTime;
  } // namespace node

  template <>
  struct node_declaration_traits<node::FrameTime>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "FrameTime",
        {},
        {"value"},
        "Constructor of time value. This node construct new FrameTime object "
        "from argument, or creates FrameTime object which represents current "
        "time applied to the tree.");
    }
  };
} // namespace yave