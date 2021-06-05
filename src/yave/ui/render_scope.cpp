//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/render_scope.hpp>
#include <yave/ui/render_context.hpp>
#include <yave/ui/layout_context.hpp>
#include <yave/ui/viewport.hpp>

namespace yave::ui {

  render_scope::render_scope(
    render_context& rctx,
    layout_context& lctx,
    const ui::viewport& vp,
    std::optional<ui::render_layer>& out)
    : m_rctx {rctx}
    , m_lctx {lctx}
    , m_out {out}
    , m_win {vp}
    , m_window_offset {lctx.get_offset(vp)}
    , m_window_size {lctx.get_size(vp)}
    , m_window_pos {0, 0}
    , m_layer {}
  {
    m_layer = create_layer();
  }

  render_scope::render_scope(
    render_context& rctx,
    layout_context& lctx,
    const ui::window& win,
    const render_scope& parent,
    ui::render_layer rl,
    std::optional<ui::render_layer>& out)
    : m_rctx {rctx}
    , m_lctx {lctx}
    , m_out {out}
    , m_win {win}
    , m_window_offset {lctx.get_offset(win)}
    , m_window_size {lctx.get_size(win)}
    , m_window_pos {parent.window_pos() + m_window_offset}
    , m_layer {std::move(rl)}
  {
    m_layer->rebind_scope(*this);
  }

  render_scope::~render_scope() noexcept
  {
    m_out = std::move(m_layer);
  }

  auto render_scope::render_ctx() -> render_context&
  {
    return m_rctx;
  }

  auto render_scope::render_ctx() const -> const render_context&
  {
    return m_rctx;
  }

  auto render_scope::enter_child(const ui::window& w, ui::render_layer&& layer)
    -> ui::render_layer
  {
    return m_rctx.render_child_window(w, *this, std::move(layer), {});
  }

  void render_scope::enter_child(const ui::window& w)
  {
    m_layer = enter_child(w, std::move(*m_layer));
  }

  auto render_scope::window() const -> const ui::window&
  {
    return m_win;
  }

  auto render_scope::window_pos() const -> vec
  {
    return m_window_pos;
  }

  auto render_scope::window_offset() const -> vec
  {
    return m_window_offset;
  }

  auto render_scope::window_size() const -> size
  {
    return m_window_size;
  }

  auto render_scope::create_layer() const -> ui::render_layer
  {
    return ui::render_layer(*this);
  }

  auto render_scope::get_layer() -> ui::render_layer&
  {
    return *m_layer;
  }

} // namespace yave::ui