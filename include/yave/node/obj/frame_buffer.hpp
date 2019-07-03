//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/data/obj/frame_buffer.hpp>
#include <yave/node/obj/function.hpp>
#include <yave/node/core/get_info.hpp>

namespace yave {

  /// Node function for frame buffer
  struct FrameBufferConstructor
    : NodeFunction<FrameBufferConstructor, FrameBuffer>
  {
    FrameBufferConstructor(frame_buffer_manager& mngr)
      : manager {mngr}
    {
    }

    return_type code() const
    {
      return make_object<FrameBuffer>(manager);
    }

    frame_buffer_manager& manager;
  };

  /// Getter function for FrameBufferConstructor
  struct FrameBufferConstructorGetterFunction
    : Function<
        FrameBufferConstructorGetterFunction,
        PrimitiveContainer,
        FrameBufferConstructor>
  {
    FrameBufferConstructorGetterFunction(frame_buffer_manager& mngr)
      : manager {mngr}
    {
    }

    return_type code() const
    {
      return make_object<FrameBufferConstructor>(manager);
    }

    frame_buffer_manager& manager;
  };

  template <>
  struct node_function_info_traits<FrameBufferConstructor>
  {
    static node_info get_node_info()
    {
      return node_info("FrameBuffer", {}, {"value"});
    }

    static bind_info get_bind_info(frame_buffer_manager& mngr)
    {
      return bind_info(
        "FrameBuffer",
        {},
        "value",
        make_object<FrameBufferConstructorGetterFunction>(mngr),
        "FrameBufferConstructor");
    }
  };
} // namespace yave