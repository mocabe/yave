//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/window_manager.hpp>
#include <yave/wm/root_window.hpp>

#include <yave/support/log.hpp>

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

  public:
    void init()
    {
      // init root window
      root_win = std::make_unique<root_window>();
    }

    impl(wm::window_manager& wmngr)
      : wm {wmngr}
    {
      init_logger();
      init();
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

    bool exists(uid id)
    {
      return get_window(id);
    }

  public:
    void update(editor::data_context& dctx, editor::view_context& vctx)
    {
      root_win->update(dctx, vctx);
    }

    void draw(editor::data_context& dctx, editor::view_context& vctx)
    {
      root_win->draw(dctx, vctx);
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

  void window_manager::update(
    editor::data_context& dctx,
    editor::view_context& vctx)
  {
    return m_pimpl->update(dctx, vctx);
  }

  void window_manager::draw(
    editor::data_context& dctx,
    editor::view_context& vctx)
  {
    return m_pimpl->draw(dctx, vctx);
  }
} // namespace yave::wm