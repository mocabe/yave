//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>

#include <yave/rts/function.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/obj/frame_time/frame_time.hpp>

namespace yave {

  namespace node {
    /// Pesudo primitive node for frame time.
    struct Time;
  } // namespace node

  namespace modules::_std::core {
    /// Get time from demand
    struct GetCurrentTime : Function<GetCurrentTime, FrameDemand, FrameTime>
    {
      return_type code() const
      {
        return make_object<FrameTime>(eval_arg<0>()->time);
      }
    };
  } // namespace modules::_std::core

  template <>
  struct node_declaration_traits<node::Time>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "Time",
        "Constructor of time value. This node construct new FrameTime object "
        "from argument, or creates FrameTime object which represents current "
        "time applied to the tree.",
        "std::time",
        {"value"},
        {"value"},
        {{0, make_object<modules::_std::core::GetCurrentTime>()}});
    }
  };
} // namespace yave