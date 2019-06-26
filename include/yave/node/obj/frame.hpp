//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/rts.hpp>
#include <yave/data/obj/frame.hpp>
#include <yave/node/core/get_info.hpp>

namespace yave {

  /// Primitive-ish node for Frame.
  struct FrameConstructor : Function<FrameConstructor, Frame, Frame>
  {
    return_type code() const
    {
      // reflect frame value
      return eval_arg<0>();
    }
  };

  // info traits
  template <>
  struct node_function_info_traits<FrameConstructor>
  {
    static node_info get_node_info()
    {
      return node_info("Frame", {}, {"value"});
    }

    static bind_info get_bind_info()
    {
      return bind_info(
        "Frame",
        {},
        "value",
        make_object<FrameConstructor>(),
        "FrameConstructor");
    }
  };
} // namespace yave