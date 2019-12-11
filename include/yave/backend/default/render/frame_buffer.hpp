//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/default/config.hpp>
#include <yave/node/decl/frame_buffer.hpp>
#include <yave/lib/frame_buffer/frame_buffer_manager.hpp>

namespace yave {

  namespace backends::default_render {

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

  } // namespace backends::default_render

  template <>
  struct node_definition_traits<node::FrameBuffer, backend_tags::default_render>
  {
    static auto get_node_definitions(frame_buffer_manager* mngr)
      -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::FrameBuffer>();
      return {node_definition(
        info.name(),
        info.output_sockets()[0],
        make_object<
          yave::backends::default_render::FrameBufferConstructorGetterFunction>(
          mngr),
        info.name())};
    }
  };
} // namespace yave