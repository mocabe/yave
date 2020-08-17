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
    auto root() -> root_window*
    {
      return root_win.get();
    }

  public:
    auto get_window(uid id) -> window*
    {
      auto rec = [id](auto&& self, window* w) -> window* {
        if (w->id() == id)
          return w;

        for (auto&& c : w->children()) {
          auto cw = w->as_mut_child(c);
          if (auto r = self(cw))
            return r;
        }

        return nullptr;
      };

      return fix_lambda(rec)(root_win.get());
    }

    auto get_viewport(uid id) -> viewport_window*
    {
      auto rec = [id](auto&& self, window* w) -> bool {
        if (w->id() == id)
          return true;

        for (auto&& c : w->children()) {
          auto cw = w->as_mut_child(c);
          if (self(cw))
            return true;
        }
        return false;
      };

      for (auto&& vp : root_win->viewports()) {
        if (fix_lambda(rec)(vp))
          return vp;
      }
      return nullptr;
    }

    bool exists(uid id)
    {
      return get_window(id);
    }

  public:
    auto screen_pos(const window* win) -> glm::vec2
    {
      assert(exists(win->id()));

      const window* w = win;

      glm::vec2 p = w->pos();

      while ((w = w->parent()))
        p += w->pos();

      return p;
    }

    bool intersects(const window* win, const glm::vec2& pos)
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

  auto window_manager::screen_pos(const window* win) const -> glm::vec2
  {
    return m_pimpl->screen_pos(win);
  }

  bool window_manager::intersects(const window* win, const glm::vec2& pos) const
  {
    return m_pimpl->intersects(win, pos);
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