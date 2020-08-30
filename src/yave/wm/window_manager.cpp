//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/window_manager.hpp>
#include <yave/wm/root_window.hpp>
#include <yave/wm/viewport_window.hpp>

#include <yave/support/log.hpp>
#include <tl/optional.hpp>

#include <map>
#include <algorithm>

YAVE_DECL_G_LOGGER(window_manager)

namespace yave::wm {

  class window_manager::impl
  {
    window_manager& wm;

  public:
    /// window tree
    std::unique_ptr<root_window> root_win;
    /// current key focus
    uid key_focus;

  public:
    impl(wm::window_manager& wmngr)
      : wm {wmngr}
      , root_win {new root_window(wm)}
    {
      init_logger();
    }

  public:
    auto root() const -> root_window*
    {
      return root_win.get();
    }

  public:
    auto get_window(uid id) const -> window*
    {
      auto rec = [id](auto&& self, window* w) -> window* {
        if (w->id() == id)
          return w;

        for (auto&& c : w->children()) {
          if (auto r = self(c))
            return r;
        }

        return nullptr;
      };

      return fix_lambda(rec)(root());
    }

    auto get_viewport(uid id) -> viewport_window*
    {
      auto rec = [id](auto&& self, window* w) -> bool {
        if (w->id() == id)
          return true;

        for (auto&& c : w->children()) {
          if (self(c))
            return true;
        }
        return false;
      };

      for (auto&& vp : root()->viewports()) {
        if (fix_lambda(rec)(vp))
          return vp;
      }
      return nullptr;
    }

    auto get_viewport(const window* w) -> viewport_window*
    {
      if (!w || !w->parent())
        return nullptr;

      while (w->parent()->parent())
        w = w->parent();

      if (w->parent() == root()) {
        for (auto&& vp : root()->children())
          if (w == vp)
            return static_cast<viewport_window*>(vp);
      }
      return nullptr;
    }

    // form ID
    bool exists(uid id) const
    {
      return get_window(id);
    }

    // from valid pointer
    bool exists(const window* w) const
    {
      if (!w)
        return false;

      while (w->parent())
        w = w->parent();

      return w == root();
    }

  public:
    bool is_child(const window* c, const window* p) const
    {
      if (!exists(c) || !exists(p))
        return false;

      const window* w = c;

      if (!w || !w->parent())
        return false;

      while ((w = w->parent()))
        if (w == p)
          return true;

      return false;
    }

    bool is_parent(const window* p, const window* c) const
    {
      return is_child(c, p);
    }

  public:
    auto screen_pos(const window* win) -> std::optional<glm::vec2>
    {
      if (!exists(win))
        return std::nullopt;

      const window* w = win;

      glm::vec2 p = w->pos();

      while ((w = w->parent()) && w->parent() != root()) {
        p += w->pos();
      }

      return p;
    }

    bool hit_test(const window* win, const glm::vec2& pos)
    {
      assert(exists(win->id()));

      // check if spos is inside of bbox of all prents
      auto rec = [&](auto&& self, const window* w) -> tl::optional<glm::vec2> {
        if (w->parent()) {
          return self(w->parent())
            .and_then([&](auto ppos) -> tl::optional<glm::vec2> {
              auto p1 = ppos + w->pos();
              auto p2 = p1 + w->size();
              // bbox check
              if (p1.x <= pos.x && pos.x <= p2.x)
                if (p1.y <= pos.y && pos.y <= p2.y)
                  return tl::make_optional(p1);
              // fail
              return tl::nullopt;
            });
        }
        // root window
        return tl::make_optional(glm::vec2 {0, 0});
      };

      return fix_lambda(rec)(win).has_value();
    }

    bool should_close()
    {
      return root_win->should_close();
    }

    auto get_key_focus() -> window*
    {
      if (key_focus == uid())
        return nullptr;

      if (auto w = get_window(key_focus))
        return w;

      Info(g_logger, "window which had key focus no longer exists");
      key_focus = {};
      return nullptr;
    }

    void set_key_focus(window* win)
    {
      assert(exists(win->id()));
      key_focus = win->id();
    }

    void exec(editor::data_context& dctx, editor::view_context& vctx)
    {
      return root_win->exec(dctx, vctx);
    }
  };

  window_manager::window_manager()
    : m_pimpl {std::make_unique<impl>(*this)}
  {
  }

  window_manager::~window_manager() noexcept = default;

  auto window_manager::root() -> root_window*
  {
    return m_pimpl->root();
  }

  auto window_manager::root() const -> const root_window*
  {
    return m_pimpl->root();
  }

  bool window_manager::exists(uid id) const
  {
    return m_pimpl->exists(id);
  }

  auto window_manager::get_window(uid id) -> window*
  {
    return m_pimpl->get_window(id);
  }

  auto window_manager::get_window(uid id) const -> const window*
  {
    return m_pimpl->get_window(id);
  }

  auto window_manager::get_viewport(uid id) const -> const viewport_window*
  {
    return m_pimpl->get_viewport(id);
  }

  auto window_manager::get_viewport(uid id) -> viewport_window*
  {
    return m_pimpl->get_viewport(id);
  }

  bool window_manager::exists(const window* w) const
  {
    return m_pimpl->exists(w);
  }

  auto window_manager::get_viewport(const window* w) const
    -> const viewport_window*
  {
    return m_pimpl->get_viewport(w);
  }

  auto window_manager::get_viewport(const window* w) -> viewport_window*
  {
    return m_pimpl->get_viewport(w);
  }

  bool window_manager::is_child(const window* c, const window* p) const
  {
    return m_pimpl->is_child(c, p);
  }

  bool window_manager::is_parent(const window* p, const window* c) const
  {
    return m_pimpl->is_parent(p, c);
  }

  auto window_manager::screen_pos(const window* win) const
    -> std::optional<glm::vec2>
  {
    return m_pimpl->screen_pos(win);
  }

  bool window_manager::hit_test(const window* win, const glm::vec2& pos) const
  {
    return m_pimpl->hit_test(win, pos);
  }

  bool window_manager::should_close() const
  {
    return m_pimpl->should_close();
  }

  auto window_manager::get_key_focus() const -> window*
  {
    return m_pimpl->get_key_focus();
  }

  void window_manager::set_key_focus(window* win)
  {
    m_pimpl->set_key_focus(win);
  }

  void window_manager::exec(
    editor::data_context& dctx,
    editor::view_context& vctx)
  {
    return m_pimpl->exec(dctx, vctx);
  }

} // namespace yave::wm