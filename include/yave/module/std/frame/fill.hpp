//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>
#include <yave/lib/vulkan/offscreen_compositor.hpp>
#include <yave/lib/frame_buffer/frame_buffer_manager.hpp>

namespace yave {

  namespace node {
    struct FillFrame;
  }

  template <>
  struct node_declaration_traits<node::FillFrame>
  {
    static auto get_node_declaration(frame_buffer_manager& fbm)
      -> node_declaration;
  };

  template <>
  struct node_definition_traits<node::FillFrame, modules::_std::tag>
  {
    static auto get_node_definitions(
      frame_buffer_manager& fbm,
      vulkan::rgba32f_offscreen_compositor& compositor)
      -> std::vector<node_definition>;
  };
}