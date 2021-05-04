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
    if (is_registered())
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

  bool window::is_registered() const
  {
    assert(m_registered == (bool)m_wm);
    return m_registered;
  }

  void window::invalidate()
  {
    if (is_registered())
      m_wm->invalidate_window(*this, {});
    else
      log_warning("invalidate() on inactive window");
  }

  bool window::is_invalidated() const
  {
    return m_invalidated;
  }

  void window::show()
  {
    if (is_registered())
      m_wm->show_window(*this, {});
    else
      log_warning("window::show() for inactive window");
  }

  void window::hide()
  {
    if (is_registered())
      m_wm->hide_window(*this, {});
    else
      log_warning("window::hide() for inactive window");
  }

  bool window::is_visible() const
  {
    return m_visible;
  }

  auto window::window_manager() -> ui::window_manager&
  {
    if (!is_registered())
      throw std::runtime_error(
        "called window::window_manager() on unregistered window");

    return *m_wm;
  }

  auto window::window_manager() const -> const ui::window_manager&
  {
    if (!is_registered())
      throw std::runtime_error(
        "called window::window_manager() on unregistered window");

    return *m_wm;
  }

  auto window::add_child(size_t idx, unique<window> win) -> window&
  {
    if (!win || win->has_parent())
      throw std::invalid_argument("window::add_child(): invalid window");

    auto& ws = m_children;

    idx = std::clamp(idx, size_t(0), ws.size());

    auto ptr = win.get();
    ws.insert(std::next(ws.begin(), idx), std::move(win));
    ptr->m_parent = this;

    if (is_registered())
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

    if (is_registered())
      m_wm->unregister_window(*ret, {});

    return ret;
  }

  void window::remove_child(const window& w)
  {
    (void)detach_child(w);
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

} // namespace yave::ui