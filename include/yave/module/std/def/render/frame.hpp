//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/config.hpp>
#include <yave/module/std/decl/render/frame.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/frame_buffer/frame_buffer.hpp>
#include <yave/lib/frame_buffer/frame_buffer_manager.hpp>

namespace yave {

  namespace modules::_std::render {

    struct FrameBufferConstructor
      : NodeFunction<FrameBufferConstructor, FrameBuffer>
    {
      FrameBufferConstructor(const object_ptr<const FrameBufferPool>& pool)
        : pool {pool}
      {
      }

      return_type code() const
      {
        return make_object<FrameBuffer>(pool);
      }

      object_ptr<const FrameBufferPool> pool;
    };
  } // namespace modules::_std::render

  template <>
  struct node_definition_traits<node::Frame, modules::_std::tag>
  {
    static auto get_node_definitions(frame_buffer_manager& mngr)
    {
      auto info = get_node_declaration<node::Frame>();
      return std::vector {node_definition(
        info.name(),
        0,
        make_object<modules::_std::render::FrameBufferConstructor>(
          mngr.get_pool_object()),
        info.description())};
    }
  };
} // namespace yave