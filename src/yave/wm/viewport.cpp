//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/viewport.hpp>
#include <yave/wm/window_traverser.hpp>
#include <yave/wm/system_events.hpp>

#include <yave/editor/data_context.hpp>
#include <yave/editor/view_context.hpp>

#include <yave/support/log.hpp>
YAVE_DECL_G_LOGGER(viewport)

namespace yave::wm {

  void viewport::_emit_viewport_events(
    editor::data_context& dctx,
    editor::view_context& vctx)
  {
    // viewport resize
    {
      auto old_size = size();
      auto new_size = fvec2(m_win.size());
      if (old_size != new_size) {
        auto e = std::make_unique<events::resize>(new_size);
        auto d = resize_event_dispatcher(
          std::forward_as_tuple(std::move(e), dctx, vctx),
          std::forward_as_tuple(id()));
        d.dispatch(this);
      }
    }
    // viewport move
    {
      auto old_pos = pos();
      auto new_pos = fvec2(m_win.pos());
      if (old_pos != new_pos) {
        auto e = std::make_unique<events::move>(new_pos);
        auto d = move_event_dispatcher(
          std::forward_as_tuple(std::move(e), dctx, vctx),
          std::forward_as_tuple(id()));
        d.dispatch(this);
      }
    }
  }

  void viewport::_emit_io_events(
    editor::data_context& dctx,
    editor::view_context& vctx)
  {
    m_io.update();
    m_key_event_emitter.dispatch_events(this, dctx, vctx);
    m_mouse_event_emitter.dispatch_events(this, dctx, vctx);
  }

  viewport::viewport(wm::window_manager& wm, glfw::glfw_window&& win)
    : viewport_window(win.title(), win.pos(), win.size())
    , m_wm {wm}
    , m_win {std::move(win)}
    , m_io {m_win}
    , m_key_event_emitter {m_io, m_wm}
    , m_mouse_event_emitter {m_io, m_wm}
  {
    init_logger();

    Info(
      g_logger,
      "Initialized viewport: pos({},{}), size({},{})",
      pos().x,
      pos().y,
      size().x,
      size().y);
  }

  auto viewport::set_layout(std::unique_ptr<layout_window>&& layout)
    -> layout_window*
  {
    assert(children().size() < 2);

    if (!children().empty())
      remove_any_window(children().front()->id());

    auto ret = layout.get();
    add_any_window(0, std::move(layout));
    return ret;
  }

  auto viewport::detach_layout() -> std::unique_ptr<layout_window>
  {
    if (children().empty())
      return nullptr;

    auto tmp = detach_any_window(children().front()->id());
    return std::unique_ptr<layout_window>(
      static_cast<layout_window*>(tmp.release()));
  }

  void viewport::remove_layout()
  {
    (void)detach_layout();
  }

  auto viewport::layout() -> layout_window*
  {
    if (children().empty())
      return nullptr;

    return static_cast<layout_window*>(as_mut_child(children().front()));
  }

  auto viewport::layout() const -> const layout_window*
  {
    if (children().empty())
      return nullptr;

    return static_cast<const layout_window*>(children().front().get());
  }

  bool viewport::should_close() const
  {
    return m_win.should_close();
  }

  auto viewport::refresh_rate() const -> uint32_t
  {
    return m_win.refresh_rate();
  }

  void viewport::exec(editor::data_context& dctx, editor::view_context& vctx)
  {
    // update stage
    update(dctx, vctx);
    // draw stage
    draw(dctx, vctx);
    // event handling stage
    m_win.glfw_ctx().poll_events();
    _emit_viewport_events(dctx, vctx);
    _emit_io_events(dctx, vctx);
  }

  void viewport::update(editor::data_context& dctx, editor::view_context& vctx)
  {
    for (auto&& c : children()) {
      as_mut_child(c)->update(dctx, vctx);
    }
  }

  void viewport::draw(
    const editor::data_context& dctx,
    const editor::view_context& vctx) const
  {
    for (auto&& c : children()) {
      c->draw(dctx, vctx);
    }
  }

  void viewport::on_resize(
    wm::events::resize& e,
    const editor::data_context& data_ctx,
    const editor::view_context& view_ctx) const
  {
    view_ctx.push(editor::make_window_view_command(
      *this, [size = e.size()](auto&& w) { w.set_size(size); }));

    // resize layout
    for (auto&& c : children()) {
      c->on_resize(e, data_ctx, view_ctx);
    }

    e.accept();
    Info(g_logger, "[resize] {},{}", e.size().x, e.size().y);
  }

  void viewport::on_move(
    wm::events::move& e,
    const editor::data_context& data_ctx,
    const editor::view_context& view_ctx) const
  {
    view_ctx.push(editor::make_window_view_command(
      *this, [pos = e.pos()](auto&& w) { w.set_pos(pos); }));

    e.accept();
    Info(g_logger, "[move] {},{}", e.pos().x, e.pos().y);
  }

  void viewport::on_mouse_click(
    wm::events::mouse_click& e,
    const editor::data_context& data_ctx,
    const editor::view_context& view_ctx) const
  {
    e.accept();
    Info(g_logger, "[click] {},{}", e.pos().x, e.pos().y);
  }
  void viewport::on_mouse_double_click(
    wm::events::mouse_double_click& e,
    const editor::data_context& data_ctx,
    const editor::view_context& view_ctx) const
  {
    e.accept();
    Info(g_logger, "[dbl click] {},{}", e.pos().x, e.pos().y);
  }

  void viewport::on_mouse_press(
    wm::events::mouse_press& e,
    const editor::data_context& data_ctx,
    const editor::view_context& view_ctx) const
  {
    e.accept();
    Info(g_logger, "[press] {},{}", e.pos().x, e.pos().y);
  }

  void viewport::on_mouse_release(
    wm::events::mouse_release& e,
    const editor::data_context& data_ctx,
    const editor::view_context& view_ctx) const
  {
    e.accept();
    Info(g_logger, "[release] {},{}", e.pos().x, e.pos().y);
  }

  void viewport::on_mouse_hover(
    wm::events::mouse_hover& e,
    const editor::data_context& data_ctx,
    const editor::view_context& view_ctx) const
  {
    e.accept();
  }

  void viewport::on_key_press(
    wm::events::key_press& e,
    const editor::data_context& data_ctx,
    const editor::view_context& view_ctx) const
  {
    e.accept();
  }

  void viewport::on_key_char(
    wm::events::key_char& e,
    const editor::data_context& data_ctx,
    const editor::view_context& view_ctx) const
  {
    Info(
      g_logger,
      "{} chars: {}",
      e.chars().size(),
      (const char*)e.chars().data());

    e.accept();
  }
} // namespace yave::wm