//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/default/config.hpp>
#include <yave/node/obj/frame_buffer.hpp>
#include <yave/lib/frame_buffer/frame_buffer_manager.hpp>
#include <yave/obj/frame_buffer/frame_buffer.hpp>
#include <yave/node/core/function.hpp>

namespace yave {

  namespace backend::default_render {

    /// Node function for frame buffer
    struct FrameBufferConstructor
      : NodeFunction<FrameBufferConstructor, FrameBuffer>
    {
      FrameBufferConstructor(const object_ptr<FrameBufferPool>& pl)
        : pool {pl}
      {
      }

      return_type code() const
      {
        return make_object<FrameBuffer>(pool);
      }

      object_ptr<FrameBufferPool> pool;
    };

    /// Getter function for FrameBufferConstructor
    struct FrameBufferConstructorGetterFunction
      : Function<
          FrameBufferConstructorGetterFunction,
          PrimitiveContainer,
          FrameBufferConstructor>
    {
      FrameBufferConstructorGetterFunction(frame_buffer_manager* mngr)
        : manager {mngr}
      {
      }

      return_type code() const
      {
        return make_object<FrameBufferConstructor>(manager->get_pool_object());
      }

      frame_buffer_manager* manager;
    };

  } // namespace backend::default_render

  template <>
  struct bind_info_traits<node::FrameBuffer, backend::tags::default_render>
  {
    static bind_info get_bind_info(frame_buffer_manager* mngr)
    {
      auto info = get_node_info<node::FrameBuffer>();
      return bind_info(
        info.name(),
        info.input_sockets(),
        info.output_sockets()[0],
        make_object<
          yave::backend::default_render::FrameBufferConstructorGetterFunction>(
          mngr),
        info.name());
    }
  };
} // namespace yave