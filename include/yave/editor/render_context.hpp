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
#include <span>

namespace yave::editor {

  /// RAII drawer
  class window_drawer
  {
    friend class render_context;

    /// context
    render_context& m_render_ctx;

    /// context
    const view_context& m_view_ctx;
    /// window
    const wm::window* m_window;

    /// window screen pos
    const fvec2 m_window_pos;

    /// draw list for this window
    draw_list m_draw_list;

  private:
    window_drawer(
      render_context& render_ctx,
      const view_context& view_ctx,
      const wm::window* win);

  public:
    ~window_drawer() noexcept;

  public:
    /// add rect
    void add_rect(const fvec2& p1, const fvec2& p2, const fvec4& col);
    /// add filled polygon
    void add_polygon(const std::span<fvec2>& ps, const fvec4& col);
    /// add filled rect
    void add_rect_filled(const fvec2& p1, const fvec2& p2, const fvec4& col);
  };

  /// editor render context
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
    /// start render pass
    void begin_frame(editor::view_context& view_ctx);
    /// end render pass
    void end_frame();
    /// render
    void render();

  public:
    /// window drawer
    auto create_window_drawer(const wm::window* win) -> window_drawer;

  private: // for window_drawer
    friend class window_drawer;
    /// push clip rect stack
    void push_clip_rect(const glm::vec2& p1, const glm::vec2& p2);
    /// pop clip rect stack
    void pop_clip_rect();
    /// get current clip rect
    auto get_clip_rect() -> draw_clip;
    /// add draw list
    void add_draw_list(draw_list&& dl);
    /// default tex
    auto get_default_texture() -> draw_tex;

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