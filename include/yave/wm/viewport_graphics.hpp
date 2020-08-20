//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/render/render_context.hpp>

namespace yave::wm {

  class viewport_graphics
  {
    render::render_context render_ctx;

  public:
    viewport_graphics(vulkan::vulkan_context& vk, glfw::glfw_window& win)
      : render_ctx {vk, win}
    {
    }

    auto& render_context()
    {
      return render_ctx;
    }
  };
} // namespace yave::wm