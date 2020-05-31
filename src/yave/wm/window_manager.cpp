//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/wm/window_manager.hpp>
#include <yave/wm/root_window.hpp>
#include <yave/wm/viewport_window.hpp>
#include <yave/editor/data_context.hpp> 
#include <yave/lib/glfw/glfw_context.hpp>
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
    /// vulkan context ref
    vulkan::vulkan_context& vk_ctx;
    /// glfw context
    glfw::glfw_context& glfw_ctx;
    /// window tree
    std::unique_ptr<root_window> root_win;

  public:
    void init()
    {
      // init root window
      root_win = std::make_unique<root_window>(wm);
    }

    impl(
      wm::window_manager& wmngr,
      vulkan::vulkan_context& vkctx,
      glfw::glfw_context& glfwctx)
      : wm {wmngr}
      , vk_ctx {vkctx}
      , glfw_ctx {glfwctx}
    {
      init_logger();
      init();
    }

  public:
    auto get_window(uid id) -> window*
    {
      auto rec = [&](auto&& self, window* w) -> window* {
        if (w->id() == id)
          return w;

        for (auto&& c : w->children()) {
          auto cw = w->as_mut_child(c);
          if (self(cw))
            return cw;
        }

        return nullptr;
      };

      return fix_lambda(rec)(root_win.get());
    }

    bool exists(uid id)
    {
      return get_window(id);
    }

    auto root() -> root_window*
    {
      return root_win.get();
    }

    bool should_close() const
    {
      return root_win->should_close();
    }

  public:
    auto screen_pos(const window* win) const -> fvec2
    {
      const window* w = win;

      fvec2 p = w->pos();

      while ((w = w->parent()))
        p += w->pos();

      return p;
    }

    bool validate_pos(const window* win, const fvec2& pos) const
    {
      // check if spos is inside of bbox of all prents
      auto rec = [&](auto&& self, const window* w) -> tl::optional<fvec2> {
        if (w->parent()) {
          return self(w->parent())
            .and_then([&](auto ppos) -> tl::optional<fvec2> {
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
        return tl::make_optional(fvec2 {0, 0});
      };

      return fix_lambda(rec)(win).has_value();
    }

  public:
    void update(editor::data_context& dctx, editor::view_context& vctx)
    {
      // lock data thread
      auto lck = dctx.lock();
      // poll window events
      glfw_ctx.poll_events();
      // update
      root_win->update(dctx, vctx);
    }

    void render(editor::data_context& dctx, editor::view_context& vctx)
    {
      root_win->render(dctx, vctx);
    }

    void events(editor::data_context& dctx, editor::view_context& vctx)
    {
      root_win->events(dctx, vctx);
    }

  public:
    void dispatch(window_visitor& visitor, window_traverser& traverser)
    {
      traverser.traverse(root_win.get(), visitor);
    }

  public:
    auto add_viewport(uint32_t width, uint32_t height, std::u8string name)
      -> viewport_window*
    {
      return root_win->add_viewport(width, height, name, vk_ctx, glfw_ctx);
    }

    void remove_viewport(uid id)
    {
      root_win->remove_viewport(id);
    }
  };

  window_manager::window_manager(
    vulkan::vulkan_context& vkctx,
    glfw::glfw_context& glfwctx)
    : m_pimpl {std::make_unique<impl>(*this, vkctx, glfwctx)}
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

  auto window_manager::get_window(uid id) -> window*
  {
    return m_pimpl->get_window(id);
  }

  auto window_manager::get_window(uid id) const -> const window*
  {
    return m_pimpl->get_window(id);
  }

  bool window_manager::should_close() const
  {
    return m_pimpl->should_close();
  }

  bool window_manager::exists(uid id) const
  {
    return m_pimpl->exists(id);
  }

  auto window_manager::screen_pos(const window* win) const -> fvec2
  {
    assert(exists(win->id()));
    return m_pimpl->screen_pos(win);
  }

  bool window_manager::validate_pos(const window* win, const fvec2& pos) const
  {
    assert(exists(win->id()));
    return m_pimpl->validate_pos(win, pos);
  }

  void window_manager::update(
    editor::data_context& dctx,
    editor::view_context& vctx)
  {
    return m_pimpl->update(dctx, vctx);
  }

  void window_manager::render(
    editor::data_context& dctx,
    editor::view_context& vctx)
  {
    return m_pimpl->render(dctx, vctx);
  }

  void window_manager::events(
    editor::data_context& dctx,
    editor::view_context& vctx)
  {
    return m_pimpl->events(dctx, vctx);
  }

  void window_manager::dispatch(
    window_visitor& visitor,
    window_traverser& traverser)
  {
    return m_pimpl->dispatch(visitor, traverser);
  }

  auto window_manager::add_viewport(
    uint32_t width,
    uint32_t height,
    std::u8string name) -> viewport_window*
  {
    return m_pimpl->add_viewport(width, height, name);
  }

  void window_manager::reomev_viewport(uid id)
  {
    m_pimpl->remove_viewport(id);
  }
} // namespace yave::wm