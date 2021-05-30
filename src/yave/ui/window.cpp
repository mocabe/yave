//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/window.hpp>
#include <yave/ui/window_manager.hpp>
#include <yave/ui/controller.hpp>
#include <yave/ui/layout_context.hpp>
#include <yave/ui/render_context.hpp>

#include <yave/support/log.hpp>

#include <algorithm>
#include <random>

YAVE_DECL_LOCAL_LOGGER(ui::window)

namespace yave::ui {

  auto wid::random() -> wid
  {
    static auto rd = std::random_device();
    static auto rn = std::mt19937_64(rd());
    return {rn()};
  }

  window::window()
  {
    // TODO: move these somewhere else?
    m_ldata = std::make_unique<ui::window_layout_data>();
    m_rdata = std::make_unique<ui::window_render_data>();
  }

  window::~window() noexcept
  {
    if (registered())
      m_wm->unregister_window(*this, {});
  }

  void window::set_parent(window& p, passkey<ui::window_manager>)
  {
    m_parent = &p;
  }

  void window::set_registered(
    bool b,
    ui::window_manager& wm,
    passkey<ui::window_manager>)
  {
    m_registered = b;
    m_wm         = b ? &wm : nullptr;
  }

  void window::set_invalidated(bool b, passkey<ui::window_manager>)
  {
    m_invalidated = b;
  }

  void window::set_visible(bool b, passkey<ui::window_event_dispatcher>)
  {
    m_visible = b;
  }

  void window::set_focused(bool b, passkey<ui::window_event_dispatcher>)
  {
    m_focused = b;
  }

  auto window::window_layout_data(passkey<layout_context>) const
    -> ui::window_layout_data&
  {
    return *m_ldata;
  }

  auto window::window_render_data(passkey<render_context>) const
    -> ui::window_render_data&
  {
    return *m_rdata;
  }

  bool window::registered() const
  {
    assert(m_registered == (bool)m_wm);
    return m_registered;
  }

  void window::invalidate()
  {
    if (registered())
      m_wm->invalidate_window(*this, {});
    else
      log_warning("invalidate() on inactive window");
  }

  bool window::invalidated() const
  {
    return m_invalidated;
  }

  void window::show()
  {
    if (registered())
      m_wm->show_window(*this, {});
    else
      log_warning("window::show() for inactive window");
  }

  void window::hide()
  {
    if (registered())
      m_wm->hide_window(*this, {});
    else
      log_warning("window::hide() for inactive window");
  }

  bool window::visible() const
  {
    return m_visible;
  }

  bool window::focused() const
  {
    return m_focused;
  }

  void window::focus()
  {
    if (registered())
      m_wm->focus_window(*this, {});
    else
      log_warning("window::focus() for inactive window");
  }

  void window::blur()
  {
    if (registered())
      m_wm->blur_window(*this, {});
    else
      log_warning("window::blur() for inactive window");
  }

  bool window::focusable() const
  {
    return m_focusable;
  }

  void window::set_focusable(bool b)
  {
    m_focusable = b;
  }

  auto window::window_manager() -> ui::window_manager&
  {
    if (!registered())
      throw std::runtime_error(
        "called window::window_manager() on unregistered window");

    return *m_wm;
  }

  auto window::window_manager() const -> const ui::window_manager&
  {
    if (!registered())
      throw std::runtime_error(
        "called window::window_manager() on unregistered window");

    return *m_wm;
  }

  auto window::add_child(u64 idx, unique<window> w) -> window&
  {
    if (!w || w->has_parent())
      throw std::invalid_argument("window::add_child(): invalid window");

    auto& ws = m_children;

    idx = std::clamp(idx, size_t(0), ws.size());

    auto ptr = w.get();
    ws.insert(std::next(ws.begin(), idx), std::move(w));
    ptr->m_parent = this;

    if (registered())
      m_wm->register_window(*ptr, {});

    return *ptr;
  }

  auto window::detach_child(const window& w) -> unique<window>
  {
    auto& ws = m_children;

    auto it = std::ranges::find_if(ws, [&](auto&& x) { return x.get() == &w; });

    if (it == ws.end())
      return nullptr;

    auto ret = std::move(*it);
    ws.erase(it);
    ret->m_parent = nullptr;

    if (registered())
      m_wm->unregister_window(*ret, {});

    return ret;
  }

  void window::remove_child(const window& w)
  {
    (void)detach_child(w);
  }

  void window::move_child(const window& w, u64 index)
  {
    if (index >= m_children.size())
      return;

    auto& ws = m_children;
    auto idx = static_cast<decltype(ws.begin())::difference_type>(index);
    auto it = std::ranges::find_if(ws, [&](auto&& x) { return x.get() == &w; });
    auto to = std::next(ws.begin(), idx);

    if (std::distance(ws.begin(), it) >= idx)
      std::rotate(to, it, std::next(it));
    else
      std::rotate(it, std::next(it), std::next(to));
  }

  void window::move_child_front(const window& w)
  {
    auto& ws = m_children;

    auto it = std::ranges::find_if(ws, [&](auto&& x) { return x.get() == &w; });

    if (it != ws.end())
      std::rotate(ws.begin(), it, std::next(it));
  }

  void window::move_child_back(const window& w)
  {
    auto& ws = m_children;

    auto it = std::ranges::find_if(ws, [&](auto&& x) { return x.get() == &w; });

    if (it != ws.end())
      std::rotate(it, std::next(it), ws.end());
  }

  auto window::add_controller(unique<controller> l) -> controller&
  {
    if (!l || l->attached())
      throw std::invalid_argument(
        "window::add_controller(): invalid controller");

    l->set_window(*this, {});
    return *m_controllers.emplace_back(std::move(l));
  }

  auto window::detach_controller(const controller& l) -> unique<controller>
  {
    auto& ls = m_controllers;

    auto it = std::ranges::find_if(ls, [&](auto&& x) { return x.get() == &l; });

    if (it == ls.end())
      return nullptr;

    auto ret = std::move(*it);
    ls.erase(it);
    ret->clear_window({});
    return ret;
  }

  void window::remove_controller(const controller& l)
  {
    (void)detach_controller(l);
  }

  void window::move_controller(const controller& c, u64 index)
  {
    if (index >= m_controllers.size())
      return;

    auto& cs = m_controllers;

    auto idx = static_cast<decltype(cs.begin())::difference_type>(index);
    auto it = std::ranges::find_if(cs, [&](auto&& x) { return x.get() == &c; });
    auto to = std::next(cs.begin(), idx);

    if (std::distance(cs.begin(), it) >= idx)
      std::rotate(to, it, std::next(it));
    else
      std::rotate(it, std::next(it), std::next(to));
  }

  void window::move_controller_back(const controller& c)
  {
    auto& cs = m_controllers;

    auto it = std::ranges::find_if(cs, [&](auto&& x) { return x.get() == &c; });

    if (it != cs.end())
      std::rotate(it, std::next(it), cs.end());
  }

  void window::move_controller_front(const controller& c)
  {
    auto& cs = m_controllers;

    auto it = std::ranges::find_if(cs, [&](auto&& x) { return x.get() == &c; });

    if (it != cs.end())
      std::rotate(cs.begin(), it, std::next(it));
  }

} // namespace yave::ui