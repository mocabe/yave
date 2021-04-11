//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/draw_list.hpp>
#include <yave/ui/vec.hpp>
#include <yave/ui/size.hpp>
#include <yave/ui/color.hpp>
#include <yave/ui/rect.hpp>
#include <yave/ui/passkey.hpp>

namespace yave::ui {

  class render_context;
  class render_scope;
  class viewport_renderer;

  /// Render layer object
  class render_layer
  {
    const ui::render_context& m_rctx;
    ui::draw_lists m_lists;
    std::vector<ui::rect> m_clip_rects;
    ui::vec m_cursor_pos;

    class priv;

  public:
    render_layer(const render_scope& scope);
    render_layer(const render_layer&) = delete;
    render_layer(render_layer&& other) noexcept;
    render_layer& operator=(render_layer&& other) noexcept;

    /// swap internal state
    void swap(render_layer& other) noexcept;

  public:
    /// Bind to different render scope.
    /// This functions resets layer's cursor and clip rect to match target
    /// window's geometry of the scope.
    void rebind_scope(const render_scope& scope);

    /// Get current cursor pos of layer.
    auto cursor_pos() const -> vec;

    /// Set new cursor pos.
    void set_cursor_pos(const vec& cursor_pos);

  public:
    /// Push new clip rect.
    /// \note New clip rect will be intersection of current and new one.
    void push_clip_rect(const rect& rec);

    /// Pop clip rect
    void pop_clip_rect();

    /// Get current clip rect
    auto clip_rect() const -> rect;

  public:
    /// Append layer data
    /// \note This does not change origin and clip rects of this layer.
    void append(render_layer&& other);

    /// Prepend layer data
    /// \note This does not change origin and clip rects of this layer.
    void prepend(render_layer&& other);

  public:
    /// Draw line.
    /// \param p1 relative to cursor pos
    /// \param p2 relative to cursor pos
    /// \param width should be positive
    /// \param color color of line
    void draw_line(
      const vec& p1,
      const vec& p2,
      const f32& width,
      const color& col);

    /// Draw rectangle polyline.
    /// \param rect relative to cursor pos
    /// \param width should be positive
    /// \param col color of rectangle
    void draw_rect(const rect& rect, const f32& width, const color& col);

  public:
    auto draw_lists(passkey<viewport_renderer>) const -> const ui::draw_lists&;
  };

} // namespace yave::ui