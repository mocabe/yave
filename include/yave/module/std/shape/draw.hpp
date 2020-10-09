//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>
#include <yave/module/std/frame/frame.hpp>
#include <yave/lib/vulkan/offscreen_compositor.hpp>

namespace yave {

  namespace node::Shape {
    /// Fill shape
    class Fill;
    /// Stroke shape
    class Stroke;
    // Draw shape to framebuffer
    class Draw;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Shape::Fill);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Shape::Stroke);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Shape::Fill, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Shape::Stroke, modules::_std::tag);

  template <>
  struct node_declaration_traits<node::Shape::Draw>
  {
    static auto get_node_declaration(data::frame_buffer_manager& fbm)
      -> node_declaration;
  };

  template <>
  struct node_definition_traits<node::Shape::Draw, modules::_std::tag>
  {
    static auto get_node_definitions(
      data::frame_buffer_manager& fbm,
      vulkan::rgba32f_offscreen_compositor& compositor)
      -> std::vector<node_definition>;
  };

} // namespace yave