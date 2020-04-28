//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/render/frame.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/frame_buffer/frame_buffer.hpp>
#include <yave/lib/frame_buffer/frame_buffer_manager.hpp>

namespace yave {

  auto node_declaration_traits<node::Frame>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Frame", "/std/render", "Create new frame buffer", {}, {"value"});
  }

  auto node_definition_traits<node::Frame, modules::_std::tag>::
    get_node_definitions(frame_buffer_manager& mngr)
      -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Frame>();
    return std::vector {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::render::FrameBufferConstructor>(
        mngr.get_pool_object()),
      info.description())};
  }
} // namespace yave