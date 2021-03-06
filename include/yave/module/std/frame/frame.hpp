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

  namespace node::Frame {
    /// frame buffer
    class Frame;
  } // namespace node::Frame

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Frame::Frame);

  template <>
  struct node_definition_traits<node::Frame::Frame, modules::_std::tag>
  {
    static auto get_node_definitions(
      data::frame_buffer_manager& mngr,
      vulkan::rgba32f_offscreen_compositor& comp)
      -> std::vector<node_definition>;
  };
}