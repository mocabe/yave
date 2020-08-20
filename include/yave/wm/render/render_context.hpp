//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/wm/render/draw.hpp>
#include <yave/lib/vulkan/window_context.hpp>
#include <yave/lib/glfw/glfw_context.hpp>

#include <glm/glm.hpp>

namespace yave::wm::render {

  /// render context
  class render_context
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    /// ctor
    render_context(vulkan::vulkan_context& vk_ctx, glfw::glfw_window& glfw_win);
    /// dtor
    ~render_context() noexcept;

  public:
    auto vulkan_context() const -> const vulkan::vulkan_context&;
    auto vulkan_context() -> vulkan::vulkan_context&;
    auto window_context() const -> const vulkan::window_context&;
    auto window_context() -> vulkan::window_context&;

  public:
    /// get default texture
    auto default_tex() -> draw_tex;

  public:
    /// start render pass
    void begin_frame();
    /// end render pass
    void end_frame();
    /// append new draw list
    void add_draw_list(draw_list&& dl);
    /// render
    void render();
  };
} // namespace yave::wm::render