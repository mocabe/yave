//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/editor/draw.hpp>
#include <yave/wm/window.hpp>

#include <yave/lib/vulkan/vulkan_context.hpp>
#include <yave/lib/vulkan/window_context.hpp>

#include <memory>

namespace yave::editor {

  /// RAII drawer
  class window_drawer
  {
    friend class render_context;

    /// context
    render_context& render_ctx;

    /// window
    const wm::window* m_window;

    /// window screen pos
    const glm::vec2 m_window_viewport_pos;

    /// clip rect stack.
    /// initial cilp rect will be matched with window area to render
    std::vector<draw_clip> m_clip_rect_stack;

    /// merged clip rect
    draw_clip m_clip_rect;

    /// draw commands for this window.
    /// coordinate is relative to viewport pos
    draw_list m_draw_list;

  private:
    window_drawer(const wm::window* win);

  public:
    ~window_drawer() noexcept;

  public:
    /// push clip rect stack
    void push_clip_rect(const glm::vec2& p1, const glm::vec2& p2);
    /// pop clip rect stack
    void pop_clip_rect();

  public:
    /// draw rect
    void add_rect(
      const glm::vec2& p1,
      const glm::vec2& p2,
      const glm::vec4& color);
  };

  /// editor render context
  class render_context
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    /// ctor
    render_context(glfw::glfw_window& glfw_win);
    /// dtor
    ~render_context() noexcept;

  public:
    /// for view context
    void begin_frame();
    /// for view context
    void end_frame();

  public:
    /// create command pass for window
    auto create_drawer(const wm::window* win) -> window_drawer;

  public:
    /// vulkan context
    auto vulkan_context() const -> const vulkan::vulkan_context&;
    /// vulkan context
    auto vulkan_context() -> vulkan::vulkan_context&;
    /// vulkan window
    auto vulkan_window() const -> const vulkan::window_context&;
    /// vulkan window
    auto vulkan_window() -> vulkan::window_context&;
  };
} // namespace yave::editor