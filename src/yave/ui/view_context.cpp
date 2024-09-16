//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/view_context.hpp>
#include <yave/ui/view_command_queue.hpp>
#include <yave/ui/window_manager.hpp>
#include <yave/ui/layout_context.hpp>
#include <yave/ui/render_context.hpp>
#include <yave/ui/glfw_context.hpp>
#include <yave/ui/vulkan_context.hpp>

#include <yave/support/log.hpp>

YAVE_DECL_LOCAL_LOGGER(ui::view_context)

namespace yave::ui {

  class view_context::impl
  {
    view_context& m_self;
    data_context& m_dctx;

  private:
    /// Command queue
    view_command_queue m_queue;
    /// exit flag
    bool m_exit = true;

  private:
    glfw_context m_glfw;
    vulkan_context m_vulkan;

  private:
    ui::layout_context m_lctx;
    ui::render_context m_rctx;
    ui::window_manager m_wm;

  public:
    auto& glfw_ctx()
    {
      return m_glfw;
    }

    auto& vulkan_ctx()
    {
      return m_vulkan;
    }

    auto& data_ctx()
    {
      return m_dctx;
    }

    auto& layout_ctx()
    {
      return m_lctx;
    }

    auto& render_ctx()
    {
      return m_rctx;
    }

  public:
    impl(view_context& self, data_context& dctx)
      : m_self {self}
      , m_dctx {dctx}
      , m_queue {self}
      , m_glfw {self}
      , m_vulkan {}
      , m_lctx {}
      , m_rctx {m_vulkan, m_glfw, m_lctx}
      , m_wm {self}
    {
      m_dctx.set_view_ctx(self, {});

      // exit when all windows are closed
      m_wm.signals.on_last_window_close.connect([&] { exit(); });
    }

    ~impl() noexcept
    {
    }

    void post(view_command cmd)
    {
      m_queue.post(std::move(cmd));
    }

    void post_delay(view_command cmd)
    {
      m_queue.post_delay(std::move(cmd));
    }

    void post_empty()
    {
      m_queue.wake();
    }

  private:
    void event()
    {
      // process pending window events
      m_wm.dispatch_pending_events({});
    }

    void render()
    {
      // process layout stage
      m_lctx.do_layout(m_wm, {});
      // process render stage
      m_rctx.do_render(m_wm, {});
      // clear invalidated flags
      m_wm.clear_invalidated({});
    }

    bool exit_requested() const
    {
      return m_exit;
    }

    bool can_poll()
    {
      return m_wm.has_pending_events({}) || !m_queue.empty();
    }

    void wait()
    {
      m_glfw.wait();
    }

    void exec()
    {
      m_queue.exec_all();
    }

    void exec_swap()
    {
      m_queue.exec_all_then_swap();
    }

  public:
    bool is_running() const
    {
      return !m_exit;
    }

    void poll()
    {
      exec();      // execute posted commands
      event();     // dispatch events
      render();    // layout and draw
      exec_swap(); // process posted commands, swap queue
    }

    void run()
    {
      m_exit = false;
      while (!exit_requested()) {
        if (can_poll())
          poll();
        else
          wait();
      }
    }

    void exit()
    {
      if (is_running())
        m_queue.post([&](auto&) { m_exit = true; });
    }

    auto& wm()
    {
      return m_wm;
    }
  };

  view_context::view_context(data_context& dctx)
    : m_pimpl {std::make_unique<impl>(*this, dctx)}
  {
  }

  view_context::~view_context() noexcept = default;

  auto view_context::glfw_ctx() -> glfw_context&
  {
    return m_pimpl->glfw_ctx();
  }

  auto view_context::glfw_ctx() const -> const glfw_context&
  {
    return m_pimpl->glfw_ctx();
  }

  auto view_context::vulkan_ctx() -> vulkan_context&
  {
    return m_pimpl->vulkan_ctx();
  }

  auto view_context::vulkan_ctx() const -> const vulkan_context&
  {
    return m_pimpl->vulkan_ctx();
  }

  auto view_context::data_ctx() -> data_context&
  {
    return m_pimpl->data_ctx();
  }

  auto view_context::data_ctx() const -> const data_context&
  {
    return m_pimpl->data_ctx();
  }

  auto view_context::layout_ctx() -> ui::layout_context&
  {
    return m_pimpl->layout_ctx();
  }

  auto view_context::layout_ctx() const -> const ui::layout_context&
  {
    return m_pimpl->layout_ctx();
  }

  auto view_context::render_ctx() -> ui::render_context&
  {
    return m_pimpl->render_ctx();
  }

  auto view_context::render_ctx() const -> const ui::render_context&
  {
    return m_pimpl->render_ctx();
  }

  void view_context::_post(view_command op) const
  {
    m_pimpl->post(std::move(op));
  }

  void view_context::_post_delay(view_command op) const
  {
    m_pimpl->post_delay(std::move(op));
  }

  void view_context::_post_empty() const
  {
    return m_pimpl->post_empty();
  }

  void view_context::run()
  {
    m_pimpl->run();
  }

  void view_context::poll()
  {
    m_pimpl->poll();
  }

  void view_context::exit()
  {
    m_pimpl->exit();
  }

  auto view_context::window_manager() const -> const ui::window_manager&
  {
    return m_pimpl->wm();
  }

  auto view_context::window_manager() -> ui::window_manager&
  {
    return m_pimpl->wm();
  }

} // namespace yave::ui