//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>
#include <yave/rts/function.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/obj/frame_time/frame_time.hpp>

namespace yave {

  namespace node {
    /// Primitive node for frame time.
    struct Time;
    /// Convert time to seconds
    struct TimeSeconds;
    /// Construct time from float seconds
    struct TimeFromSeconds;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Time);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::TimeSeconds);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::TimeFromSeconds);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Time, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::TimeSeconds, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::TimeFromSeconds, modules::_std::tag);

  namespace modules::_std::core {
    /// Get time from demand
    struct GetCurrentTime : Function<GetCurrentTime, FrameDemand, FrameTime>
    {
      return_type code() const
      {
        return eval_arg<0>()->time;
      }
    };
  } // namespace modules::_std::core

} // namespace yave