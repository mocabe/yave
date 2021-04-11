//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/draw_list.hpp>
#include <yave/ui/render_layer.hpp>

#include <optional>

namespace yave::ui {

  class window;
  class viewport;
  class render_context;
  class layout_context;

  class render_scope
  {
    // contexts ref
    ui::render_context& m_rctx;
    ui::layout_context& m_lctx;

    // out param
    std::optional<ui::render_layer>* m_out;

    // window
    const ui::window* m_win;

    // window offset
    ui::vec m_window_offset;
    // window size
    ui::size m_window_size;
    // window position
    ui::vec m_window_pos;

    /// draw layer
    /// \note being optional jsut for lazy initialization. should always have
    /// value after construction.
    std::optional<ui::render_layer> m_layer;

    friend class render_context;

    // root scope
    render_scope(
      render_context& rctx,
      layout_context& lctx,
      const ui::viewport* vp,
      std::optional<ui::render_layer>* out);

    // child scope
    render_scope(
      render_context& rctx,
      layout_context& lctx,
      const ui::window* win,
      const render_scope& parent,
      ui::render_layer rl,
      std::optional<ui::render_layer>* out);

  public:
    ~render_scope() noexcept;
    render_scope(const render_scope&) = delete;
    render_scope(render_scope&&)      = delete;
    render_scope& operator=(const render_scope&) = delete;
    render_scope& operator=(render_scope&&) = delete;

    /// Access render context
    auto render_ctx() -> render_context&;
    auto render_ctx() const -> const render_context&;

    /// enter child window scope
    /// \param layer layer object to write onto
    /// \returns draw list written in child window
    [[nodiscard]] auto enter_child(const window*, ui::render_layer&& layer)
      -> ui::render_layer;

    /// enter child window scope with current draw list
    void enter_child(const window* w);

  public:
    /// get current window
    auto window() const -> const ui::window*;
    /// position of current window
    auto window_pos() const -> vec;
    /// offset of curernt window
    auto window_offset() const -> vec;
    /// size of current window
    auto window_size() const -> size;

  public:
    /// Create new layer object from current scope.
    /// \returns new layer object with clip rect set to current window area.
    auto create_layer() const -> ui::render_layer;
    /// Get layer object bound to current scope
    auto get_layer() -> ui::render_layer&;
  };

} // namespace yave::ui