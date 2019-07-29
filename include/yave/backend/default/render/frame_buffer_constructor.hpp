//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/obj/frame_buffer.hpp>

#include <yave/data/obj/frame_buffer.hpp>
#include <yave/node/obj/function.hpp>
#include <yave/backend/default/system/config.hpp>
#include <yave/backend/default/system/frame_buffer_manager.hpp>

namespace yave {

  namespace backend::default_render {

    /// Node function for frame buffer
    struct FrameBufferConstructor
      : NodeFunction<FrameBufferConstructor, FrameBuffer>
    {
      FrameBufferConstructor(
        yave::backend::system::default_common::frame_buffer_manager& mngr)
        : manager {mngr}
      {
      }

      return_type code() const
      {
        return make_object<FrameBuffer>(manager);
      }

      yave::backend::system::default_common::frame_buffer_manager& manager;
    };

    /// Getter function for FrameBufferConstructor
    struct FrameBufferConstructorGetterFunction
      : Function<
          FrameBufferConstructorGetterFunction,
          PrimitiveContainer,
          FrameBufferConstructor>
    {
      FrameBufferConstructorGetterFunction(
        yave::backend::system::default_common::frame_buffer_manager& mngr)
        : manager {mngr}
      {
      }

      return_type code() const
      {
        return make_object<FrameBufferConstructor>(manager);
      }

      yave::backend::system::default_common::frame_buffer_manager& manager;
    };

  } // namespace backend::default_render

  template <>
  struct bind_info_traits<FrameBufferConstructor, backend::tags::default_render>
  {
    static bind_info get_bind_info(
      yave::backend::system::default_common::frame_buffer_manager& mngr)
    {
      return bind_info(
        "FrameBuffer",
        {},
        "value",
        make_object<
          yave::backend::default_render::FrameBufferConstructorGetterFunction>(
          mngr),
        "FrameBufferConstructor");
    }
  };
} // namespace yave