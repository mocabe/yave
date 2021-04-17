//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/window_manager.hpp>
#include <yave/ui/window_event_dispatcher.hpp>
#include <yave/ui/root.hpp>
#include <yave/ui/native_window.hpp>
#include <yave/ui/point.hpp>

#include <yave/support/log.hpp>

#include <map>
#include <algorithm>
#include <ranges>
#include <variant>

namespace yave::ui {

  namespace {

    struct wid_hasher
    {
      auto operator()(wid id) const noexcept -> size_t
      {
        // notice wid is already random
        return id.data;
      }
    };

  } // namespace

  class window_manager::impl
  {
    window_manager& m_self;
    view_context& m_vctx;

  private:
    /// window map
    std::unordered_map<wid, window*, wid_hasher> m_wmap;
    /// dirty(invalidated) window map
    std::unordered_map<wid, window*, wid_hasher> m_dmap;
    /// window tree
    std::unique_ptr<ui::root> m_tree;

  private:
    /// event dispatcher
    window_event_dispatcher m_event_dispatcher;

  public:
    impl(ui::window_manager& self, view_context& vctx)
      : m_self {self}
      , m_vctx {vctx}
      , m_tree {std::make_unique<ui::root>(self, passkey<window_manager>())}
      , m_event_dispatcher {self, vctx}
    {
      // register root
      register_window(m_tree.get());
    }

    auto& view_ctx()
    {
      return m_vctx;
    }

    auto root() const -> ui::root*
    {
      return m_tree.get();
    }

    // recursively register windows
    void register_window_impl(window* w)
    {
      assert(w);
      assert(!w->is_registered());

      // register
      w->set_registered(true, m_self, {});
      m_wmap.emplace(w->id(), w);

      // invalidate
      w->set_invalidated(true, {});
      m_dmap.emplace(w->id(), w);

      for (auto&& c : w->children())
        register_window_impl(c);
    }

    void register_window(window* w)
    {
      assert(w);
      assert(!w->is_registered());

      if (auto p = w->parent())
        assert(p->is_registered());

      m_self.signals.on_register(w);
      register_window_impl(w);

      if (w->is_visible())
        m_event_dispatcher.process_show_event(w);
    }

    // recursively unregister windows
    void unregister_window_impl(window* w)
    {
      assert(w);
      assert(w->is_registered());

      // unregister
      m_wmap.erase(w->id());
      w->set_registered(false, m_self, {});

      // recursively mark subtree
      for (auto&& c : w->children())
        unregister_window_impl(c);
    }

    void unregister_window(window* w)
    {
      assert(w);
      assert(w->is_registered());

      if (w->is_visible())
        m_event_dispatcher.process_hide_event(w);

      m_self.signals.on_unregister(w);
      unregister_window_impl(w);
    }

    // mark window as invalidated
    void invalidate_window(window* w)
    {
      assert(w);
      assert(w->is_registered());

      if (!w->is_invalidated()) {
        m_self.signals.on_invalidate(w);
        w->set_invalidated(true, {});
        m_dmap.emplace(w->id(), w);
      }
    }

    // clear all dirty flags
    void clear_invalidated_windows()
    {
      for (auto&& [id, w] : m_dmap) {
        (void)id;
        w->set_invalidated(false, {});
      }
      m_dmap.clear();
    }

    void show_window(window* w)
    {
      assert(w);
      assert(w->is_registered());

      if (!w->is_visible()) {
        m_event_dispatcher.process_show_event(w);
        w->set_visible(true, {});
      }
    }

    void hide_window(window* w)
    {
      assert(w);
      assert(w->is_registered());

      if (w->is_visible()) {
        m_event_dispatcher.process_hide_event(w);
        w->set_visible(false, {});
      }
    }

    auto find_window(wid id) const -> window*
    {
      if (auto it = m_wmap.find(id); it != m_wmap.end())
        return it->second;
      return nullptr;
    }

    auto find_viewport(const window* w) -> viewport*
    {
      if (!w)
        return nullptr;

      if (!w->is_registered() || w == root())
        return nullptr;

      assert(w->parent());

      while (w->parent()->parent())
        w = w->parent();

      for (auto&& vp : root()->viewports()) {
        if (vp == w)
          return vp;
      }
      return nullptr;
    }

    // form ID
    bool exists(wid id) const
    {
      return m_wmap.find(id) != m_wmap.end();
    }

  public:
    bool is_child(const window* c, const window* p) const
    {
      if (!c || !p)
        return false;

      if (!c->is_registered() || !p->is_registered())
        return false;

      const window* w = c;

      if (!w || !w->parent())
        return false;

      while ((w = w->parent()) != nullptr)
        if (w == p)
          return true;

      return false;
    }

    bool is_parent(const window* p, const window* c) const
    {
      return is_child(c, p);
    }

  public:
    void push_pos_event(GLFWwindow* win, u32 x, u32 y)
    {
      m_event_dispatcher.push_pos_event(win, x, y);
    }

    void push_size_event(GLFWwindow* win, u32 w, u32 h)
    {
      m_event_dispatcher.push_size_event(win, w, h);
    }

    void push_close_event(GLFWwindow* win)
    {
      m_event_dispatcher.push_close_event(win);
    }

    void push_refresh_event(GLFWwindow* win)
    {
      m_event_dispatcher.push_refresh_event(win);
    }

    void push_focus_event(GLFWwindow* win, bool focused)
    {
      m_event_dispatcher.push_focus_event(win, focused);
    }

    bool has_pending_events()
    {
      return m_event_dispatcher.has_pending_events();
    }

    void dispatch_pending_events()
    {
      m_event_dispatcher.dispatch_pending_events();
    }

    void push_fb_size_event(GLFWwindow* win, u32 w, u32 h)
    {
      m_event_dispatcher.push_fb_size_event(win, w, h);
    }

    void push_content_scale_event(GLFWwindow* win, f32 xs, f32 ys)
    {
      m_event_dispatcher.push_content_scale_event(win, xs, ys);
    }

    void push_maximize_event(GLFWwindow* win, bool maximized)
    {
      m_event_dispatcher.push_maximize_event(win, maximized);
    }

    void push_minimize_event(GLFWwindow* win, bool minimized)
    {
      m_event_dispatcher.push_minimize_event(win, minimized);
    }
  };

  window_manager::window_manager(view_context& vctx)
    : m_pimpl {std::make_unique<impl>(*this, vctx)}
  {
  }

  window_manager::~window_manager() noexcept = default;

  auto window_manager::view_ctx() -> view_context&
  {
    return m_pimpl->view_ctx();
  }

  bool window_manager::has_pending_events(passkey<view_context>) const
  {
    return m_pimpl->has_pending_events();
  }

  void window_manager::dispatch_pending_events(passkey<view_context>)
  {
    return m_pimpl->dispatch_pending_events();
  }

  void window_manager::clear_invalidated(passkey<view_context>)
  {
    return m_pimpl->clear_invalidated_windows();
  }

  auto window_manager::root() -> ui::root*
  {
    return m_pimpl->root();
  }

  auto window_manager::root() const -> const ui::root*
  {
    return m_pimpl->root();
  }

  bool window_manager::exists(wid id) const
  {
    return m_pimpl->exists(id);
  }

  auto window_manager::find(wid id) -> window*
  {
    return m_pimpl->find_window(id);
  }

  auto window_manager::find(wid id) const -> const window*
  {
    return m_pimpl->find_window(id);
  }

  bool window_manager::is_child(const window* c, const window* p) const
  {
    return m_pimpl->is_child(c, p);
  }

  bool window_manager::is_parent(const window* p, const window* c) const
  {
    return m_pimpl->is_parent(p, c);
  }

  void window_manager::invalidate_window(window* w, passkey<window>)
  {
    m_pimpl->invalidate_window(w);
  }

  void window_manager::register_window(window* w, passkey<window>)
  {
    m_pimpl->register_window(w);
  }

  void window_manager::unregister_window(window* w, passkey<window>)
  {
    m_pimpl->unregister_window(w);
  }

  void window_manager::show_window(window* w, passkey<window>)
  {
    m_pimpl->show_window(w);
  }

  void window_manager::hide_window(window* w, passkey<window>)
  {
    m_pimpl->hide_window(w);
  }

  void window_manager::push_pos_event(GLFWwindow* win, u32 x, u32 y)
  {
    m_pimpl->push_pos_event(win, x, y);
  }

  void window_manager::push_size_event(GLFWwindow* win, u32 w, u32 h)
  {
    m_pimpl->push_size_event(win, w, h);
  }

  void window_manager::push_fb_size_event(GLFWwindow* win, u32 w, u32 h)
  {
    m_pimpl->push_fb_size_event(win, w, h);
  }

  void window_manager::push_content_scale_event(GLFWwindow* win, f32 xs, f32 ys)
  {
    m_pimpl->push_content_scale_event(win, xs, ys);
  }

  void window_manager::push_close_event(GLFWwindow* win)
  {
    m_pimpl->push_close_event(win);
  }

  void window_manager::push_refresh_event(GLFWwindow* win)
  {
    m_pimpl->push_refresh_event(win);
  }

  void window_manager::push_focus_event(GLFWwindow* win, bool focused)
  {
    m_pimpl->push_focus_event(win, focused);
  }

  void window_manager::push_maximize_event(GLFWwindow* win, bool maximized)
  {
    m_pimpl->push_maximize_event(win, maximized);
  }

  void window_manager::push_minimize_event(GLFWwindow* win, bool minimized)
  {
    m_pimpl->push_minimize_event(win, minimized);
  }
} // namespace yave::ui