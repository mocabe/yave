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

  namespace node::Time {
    /// Primitive node for frame time.
    struct Time;
    /// Convert time to seconds
    struct Seconds;
    /// Construct time from float seconds
    struct FromSeconds;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Time::Time);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Time::Seconds);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Time::FromSeconds);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Time::Time, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Time::Seconds, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Time::FromSeconds, modules::_std::tag);

} // namespace yave