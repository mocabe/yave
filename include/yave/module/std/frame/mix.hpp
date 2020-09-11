//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>
#include <yave/lib/vulkan/offscreen_compositor.hpp>
#include <yave/data/frame_buffer/frame_buffer_manager.hpp>

namespace yave {

  namespace node {
    struct MixFrame;
  }

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::MixFrame);

  template <>
  struct node_definition_traits<node::MixFrame, modules::_std::tag>
  {
    static auto get_node_definitions(
      data::frame_buffer_manager& fbm,
      vulkan::rgba32f_offscreen_compositor& compositor)
      -> std::vector<node_definition>;
  };
} // namespace yave
