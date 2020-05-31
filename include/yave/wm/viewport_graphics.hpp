//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/window.hpp>

#include <yave/lib/glfw/glfw_context.hpp>
#include <yave/lib/vulkan/vulkan_context.hpp>

namespace yave::wm {

  /// viewoprt rendering context
  class viewport_graphics
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    /// ctor
    viewport_graphics(
      vulkan::vulkan_context& vk_ctx,
      glfw::glfw_window& glfw_win);
    /// dtor
    ~viewport_graphics() noexcept;

  public:
    /// get render context
    auto render_context() -> editor::render_context&;
  };
}