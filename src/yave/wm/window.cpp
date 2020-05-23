//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/window.hpp>
#include <yave/wm/mouse_events.hpp>
#include <yave/wm/key_events.hpp>

namespace yave::wm {

  window::window(std::string name, fvec2 pos, fvec2 size)
    : m_parent {nullptr}
    , m_id {uid::random_generate()}
    , m_children {}
    , m_name {std::move(name)}
    , m_pos {pos}
    , m_size {size}
  {
  }

  window::~window() noexcept = default;

#define YAVE_WM_DISPATCH_EVENT(E, ...)         \
  if (auto* ee = event_cast_if<events::E>(&e)) \
    on_##E(*ee, __VA_ARGS__);

  void window::emit(
    event& e,
    editor::data_context& data_ctx,
    editor::view_context& view_ctx) const
  {
    YAVE_WM_DISPATCH_EVENT(mouse_click, data_ctx, view_ctx);
    YAVE_WM_DISPATCH_EVENT(mouse_double_click, data_ctx, view_ctx);
    YAVE_WM_DISPATCH_EVENT(mouse_press, data_ctx, view_ctx);
    YAVE_WM_DISPATCH_EVENT(mouse_release, data_ctx, view_ctx);
    YAVE_WM_DISPATCH_EVENT(mouse_hover, data_ctx, view_ctx);

    YAVE_WM_DISPATCH_EVENT(key_press, data_ctx, view_ctx);
    YAVE_WM_DISPATCH_EVENT(key_release, data_ctx, view_ctx);
    YAVE_WM_DISPATCH_EVENT(key_repeat, data_ctx, view_ctx);

    on_custom_event(e, data_ctx, view_ctx);
  }

  void window::on_mouse_click(
    events::mouse_click&,
    editor::data_context&,
    editor::view_context&) const
  {
  }

  void window::on_mouse_double_click(
    events::mouse_double_click&,
    editor::data_context&,
    editor::view_context&) const
  {
  }

  void window::on_mouse_press(
    events::mouse_press&,
    editor::data_context&,
    editor::view_context&) const
  {
  }

  void window::on_mouse_release(
    events::mouse_release&,
    editor::data_context&,
    editor::view_context&) const
  {
  }

  void window::on_mouse_hover(
    events::mouse_hover&,
    editor::data_context&,
    editor::view_context&) const
  {
  }

  void window::on_key_press(
    events::key_press&,
    editor::data_context&,
    editor::view_context&) const
  {
  }

  void window::on_key_release(
    events::key_release&,
    editor::data_context&,
    editor::view_context&) const
  {
  }

  void window::on_key_repeat(
    events::key_repeat&,
    editor::data_context&,
    editor::view_context&) const
  {
  }

  void window::on_custom_event(
    event&,
    editor::data_context&,
    editor::view_context&) const
  {
  }

  auto window::screen_pos() const -> fvec2
  {
    const window* w = this;

    fvec2 p = pos();

    while ((w = w->parent()))
      p += w->pos();

    return p;
  }

} // namespace yave::wm