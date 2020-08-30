//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/window.hpp>
#include <yave/wm/mouse_events.hpp>
#include <yave/wm/key_events.hpp>
#include <yave/wm/viewport_events.hpp>
#include <yave/wm/draw_event.hpp>

#include <algorithm>
#include <iterator>

namespace yave::wm {

  window::window(std::string name, glm::vec2 pos, glm::vec2 size)
    : m_id {uid::random_generate()}
    , m_parent {nullptr}
    , m_children {}
    , m_name {std::move(name)}
    , m_pos {pos}
    , m_size {size}
  {
  }

  window::window(std::string name)
    : window(name, glm::vec2(), glm::vec2())
  {
  }

  window::~window() noexcept
  {
    for (auto&& c : m_children)
      delete c;
  }

  void window::add_child_window(size_t idx, std::unique_ptr<window>&& win)
  {
    assert(!win->m_parent);
    win->m_parent = this;

    auto& ws = m_children;
    auto it  = ws.begin();
    std::advance(it, std::clamp(idx, size_t(0), ws.size()));
    ws.emplace(it, win.release());
  }

  auto window::detach_child_window(uid id) -> std::unique_ptr<window>
  {
    auto& ws = m_children;

    std::unique_ptr<window> ret;

    auto it = std::find_if(
      ws.begin(), ws.end(), [id](auto p) { return p->id() == id; });

    if (it != ws.end()) {
      ret.reset(*it);
      ret->m_parent = nullptr;
      ws.erase(it);
    }
    return ret;
  }

  void window::remove_child_window(uid id)
  {
    (void)detach_child_window(id);
  }

  void window::move_child_window_front(uid id)
  {
    auto& ws = m_children;
    std::stable_partition(
      ws.begin(), ws.end(), [id](auto p) { return p->id() == id; });
  }

  void window::move_child_window_back(uid id)
  {
    auto& ws = m_children;
    std::stable_partition(
      ws.begin(), ws.end(), [id](auto p) { return p->id() != id; });
  }

  void window::set_name(std::string name)
  {
    m_name = std::move(name);
  }

  void window::set_pos(glm::vec2 pos)
  {
    m_pos = pos;
  }

  void window::set_size(glm::vec2 size)
  {
    m_size = size;
  }

#define YAVE_WM_DISPATCH_EVENT(E, ...)         \
  if (auto* ee = event_cast_if<events::E>(&e)) \
    return on_##E(*ee, __VA_ARGS__);

  void window::event(
    wm::event& e,
    const editor::data_context& data_ctx,
    const editor::view_context& view_ctx) const
  {
    YAVE_WM_DISPATCH_EVENT(resize, data_ctx, view_ctx);
    YAVE_WM_DISPATCH_EVENT(move, data_ctx, view_ctx);
    YAVE_WM_DISPATCH_EVENT(draw, data_ctx, view_ctx);

    YAVE_WM_DISPATCH_EVENT(mouse_click, data_ctx, view_ctx);
    YAVE_WM_DISPATCH_EVENT(mouse_double_click, data_ctx, view_ctx);
    YAVE_WM_DISPATCH_EVENT(mouse_press, data_ctx, view_ctx);
    YAVE_WM_DISPATCH_EVENT(mouse_release, data_ctx, view_ctx);
    YAVE_WM_DISPATCH_EVENT(mouse_move, data_ctx, view_ctx);
    YAVE_WM_DISPATCH_EVENT(mouse_over, data_ctx, view_ctx);
    YAVE_WM_DISPATCH_EVENT(mouse_out, data_ctx, view_ctx);
    YAVE_WM_DISPATCH_EVENT(mouse_enter, data_ctx, view_ctx);
    YAVE_WM_DISPATCH_EVENT(mouse_leave, data_ctx, view_ctx);

    YAVE_WM_DISPATCH_EVENT(key_press, data_ctx, view_ctx);
    YAVE_WM_DISPATCH_EVENT(key_release, data_ctx, view_ctx);
    YAVE_WM_DISPATCH_EVENT(key_char, data_ctx, view_ctx);

    on_custom_event(e, data_ctx, view_ctx);
  }

  void window::on_resize(
    wm::events::resize&,
    const editor::data_context&,
    const editor::view_context&) const
  {
  }

  void window::on_move(
    wm::events::move&,
    const editor::data_context&,
    const editor::view_context&) const
  {
  }

  void window::on_draw(
    wm::events::draw& e,
    const editor::data_context&,
    const editor::view_context&) const
  {
    e.ignore();
  }

  void window::on_mouse_click(
    events::mouse_click& e,
    const editor::data_context&,
    const editor::view_context&) const
  {
    e.ignore();
  }

  void window::on_mouse_double_click(
    events::mouse_double_click& e,
    const editor::data_context&,
    const editor::view_context&) const
  {
    e.ignore();
  }

  void window::on_mouse_press(
    events::mouse_press& e,
    const editor::data_context&,
    const editor::view_context&) const
  {
    e.ignore();
  }

  void window::on_mouse_release(
    events::mouse_release& e,
    const editor::data_context&,
    const editor::view_context&) const
  {
    e.ignore();
  }

  void window::on_mouse_move(
    events::mouse_move& e,
    const editor::data_context&,
    const editor::view_context&) const
  {
    e.ignore();
  }

  void window::on_mouse_over(
    wm::events::mouse_over& e,
    const editor::data_context&,
    const editor::view_context&) const
  {
    e.ignore();
  }

  void window::on_mouse_out(
    wm::events::mouse_out& e,
    const editor::data_context&,
    const editor::view_context&) const
  {
    e.ignore();
  }

  void window::on_mouse_enter(
    wm::events::mouse_enter&,
    const editor::data_context&,
    const editor::view_context&) const
  {
  }

  void window::on_mouse_leave(
    wm::events::mouse_leave&,
    const editor::data_context&,
    const editor::view_context&) const
  {
  }

  void window::on_key_press(
    events::key_press& e,
    const editor::data_context&,
    const editor::view_context&) const
  {
    e.ignore();
  }

  void window::on_key_release(
    events::key_release& e,
    const editor::data_context&,
    const editor::view_context&) const
  {
    e.ignore();
  }

  void window::on_key_char(
    wm::events::key_char& e,
    const editor::data_context&,
    const editor::view_context&) const
  {
    e.ignore();
  }

  void window::on_custom_event(
    wm::event& e,
    const editor::data_context&,
    const editor::view_context&) const
  {
    e.ignore();
  }

} // namespace yave::wm