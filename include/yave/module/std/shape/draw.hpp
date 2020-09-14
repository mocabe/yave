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

  namespace node {
    /// Fill shape
    class FillShape;
    /// Stroke shape
    class StrokeShape;
    // Draw shape to framebuffer
    class DrawShape;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::FillShape);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::StrokeShape);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::FillShape, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::StrokeShape, modules::_std::tag);

  template <>
  struct node_declaration_traits<node::DrawShape>
  {
    static auto get_node_declaration(data::frame_buffer_manager& fbm)
      -> node_declaration;
  };

  template <>
  struct node_definition_traits<node::DrawShape, modules::_std::tag>
  {
    static auto get_node_definitions(
      data::frame_buffer_manager& fbm,
      vulkan::rgba32f_offscreen_compositor& compositor)
      -> std::vector<node_definition>;
  };

} // namespace yave