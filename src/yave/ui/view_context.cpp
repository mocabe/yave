//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/view_context.hpp>
#include <yave/ui/view_command_queue.hpp>
#include <yave/ui/main_context.hpp>
#include <yave/ui/window_manager.hpp>
#include <yave/ui/layout_context.hpp>
#include <yave/ui/render_context.hpp>
#include <yave/support/log.hpp>

YAVE_DECL_LOCAL_LOGGER(ui::view_context)

namespace yave::ui {

  namespace {

    // TODO: Replace this with C++20 std::binary_semaphore
    class binary_semaphore
    {
      bool m_open = true;
      std::mutex m_mtx;
      std::condition_variable m_cond;

    public:
      void acquire()
      {
        auto lck = std::unique_lock(m_mtx);
        m_cond.wait(lck, [=] { return m_open; });
        m_open = false;
      }

      void release()
      {
        auto lck = std::unique_lock(m_mtx);
        if (!m_open) {
          m_open = true;
          m_cond.notify_one();
        }
      }
    };

  } // namespace

  class view_context::impl
  {
    view_context& m_self;
    data_context& m_dctx;
    main_context& m_mctx;

  private:
    /// Command queue
    view_command_queue m_queue;
    /// Command queue thread
    std::thread m_thread;
    /// exit flag
    bool m_exit_thread = true;
    /// refresh semaphore
    binary_semaphore m_refresh_semaphore;

  private:
    ui::layout_context m_lctx;
    ui::render_context m_rctx;
    ui::window_manager m_wm;

  public:
    auto& data_ctx()
    {
      return m_dctx;
    }

    auto& main_ctx()
    {
      return m_mctx;
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
    impl(view_context& self, main_context& mctx, data_context& dctx)
      : m_self {self}
      , m_dctx {dctx}
      , m_mctx {mctx}
      , m_queue {self}
      , m_lctx {}
      , m_rctx {mctx, m_lctx}
      , m_wm {self}
    {
      m_mctx.set_view_ctx(self, {});
      m_dctx.set_view_ctx(self, {});

      // exit when all windows are closed
      m_wm.signals.on_last_window_close.connect([&] { exit(); });
    }

    ~impl() noexcept
    {
      wait_exit();
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
      // process layout stage
      m_lctx.do_layout(m_wm, {});
      // process render stage
      m_rctx.do_render(m_wm, {});
      // clear invalidated flags
      m_wm.clear_invalidated({});
      // notify main thread
      notify_refresh_completion();
    }

    bool is_thread_running() const
    {
      return m_thread.joinable();
    }

    bool thread_exit_requested() const
    {
      return m_exit_thread;
    }

    void wait()
    {
      if (!m_wm.has_pending_events({}))
        m_queue.wait();
    }

    void exec()
    {
      m_queue.exec_all();
    }

    void exec_swap()
    {
      m_queue.exec_all_then_swap();
    }

    void wait_exit()
    {
      if (is_thread_running())
        m_thread.join();
    }

  public:
    bool is_running() const
    {
      return is_thread_running();
    }

    void run()
    {
      if (is_thread_running()) {
        log_warning("view_context::run(): event loop is already running");
        return;
      }

      // spawn view thread
      m_thread = std::thread([&] {
        // run view thread loop
        m_exit_thread = false;
        while (!thread_exit_requested()) {
          wait();      // wait new command
          exec();      // execute posted commands
          event();     // dispatch events, layout and draw
          exec_swap(); // process posted commands, swap queue
        }
        // exit main loop
        m_mctx.post([&](auto& ctx) {
          wait_exit();
          ctx.exit();
        });
      });
    }

    void exit()
    {
      if (is_thread_running())
        m_queue.post([&](auto&) { m_exit_thread = true; });
    }

    auto& wm()
    {
      return m_wm;
    }

  public:
    void wait_previous_refresh()
    {
      m_refresh_semaphore.acquire();
    }

    void notify_refresh_completion()
    {
      m_refresh_semaphore.release();
    }
  };

  view_context::view_context(main_context& mctx, data_context& dctx)
    : m_pimpl {std::make_unique<impl>(*this, mctx, dctx)}
  {
  }

  view_context::~view_context() noexcept = default;

  auto view_context::data_ctx() -> data_context&
  {
    return m_pimpl->data_ctx();
  }

  auto view_context::data_ctx() const -> const data_context&
  {
    return m_pimpl->data_ctx();
  }

  auto view_context::main_ctx() -> main_context&
  {
    return m_pimpl->main_ctx();
  }

  auto view_context::main_ctx() const -> const main_context&
  {
    return m_pimpl->main_ctx();
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

  void view_context::post_window_pos_event(GLFWwindow* win, u32 x, u32 y)
  {
    post([=](auto& ctx) { ctx.window_manager().push_pos_event(win, x, y); });
  }

  void view_context::post_window_size_event(GLFWwindow* win, u32 w, u32 h)
  {
    post([=](auto& ctx) { ctx.window_manager().push_size_event(win, w, h); });
  }

  void view_context::post_window_close_event(GLFWwindow* win)
  {
    post([=](auto& ctx) { ctx.window_manager().push_close_event(win); });
  }

  void view_context::post_window_refresh_event(GLFWwindow* win)
  {
    // HACK: To prevent failing present command multiple times on resizing
    // window (which causes visual artifacts), we block on refresh callback to
    // wait previous refresh cycle to be completed.
    m_pimpl->wait_previous_refresh();
    post([=](auto& ctx) { ctx.window_manager().push_refresh_event(win); });
  }

  void view_context::post_window_focus_event(GLFWwindow* win, bool focused)
  {
    post(
      [=](auto& ctx) { ctx.window_manager().push_focus_event(win, focused); });
  }

  void view_context::post_window_minimize_event(GLFWwindow* win, bool minimized)
  {
    post([=](auto&) { log_info("minimize event: {}", minimized); });
  }

  void view_context::post_window_maximize_event(GLFWwindow* win, bool maximized)
  {
    post([=](auto&) { log_info("maximized event: {}", maximized); });
  }

  void view_context::post_window_framebuffer_size_event(
    GLFWwindow* win,
    u32 w,
    u32 h)
  {
    post(
      [=](auto& ctx) { ctx.window_manager().push_fb_size_event(win, w, h); });
  }

  void view_context::post_window_content_scale_event(
    GLFWwindow* win,
    f32 xs,
    f32 ys)
  {
    post([=](auto& ctx) {
      ctx.window_manager().push_content_scale_event(win, xs, ys);
    });
  }

  void view_context::post_key_event(
    GLFWwindow* win,
    ui::key key,
    ui::key_action action,
    ui::key_modifier_flags mods)
  {
    post([=](auto&) {
      log_info("key event: {}, {}, {}", (int)key, (int)action, (int)mods);
    });
  }

  void view_context::post_char_event(GLFWwindow* win, u32 codepoint)
  {
    post([=](auto&) { log_info("char event: {:X}", codepoint); });
  }

  void view_context::post_mouse_event(
    GLFWwindow* win,
    ui::mouse_button button,
    ui::mouse_button_action action,
    ui::key_modifier_flags mods)
  {
    post([=](auto&) {
      log_info("mouse event: {}, {}, {}", (int)button, (int)action, (int)mods);
    });
  }

  void view_context::post_cursor_pos_event(GLFWwindow* win, f64 x, f64 y)
  {
    post([=](auto&) { log_info("cursor pos event: {}, {}", x, y); });
  }

  void view_context::post_cursor_enter_event(GLFWwindow* win, bool entered)
  {
    post([=](auto&) { log_info("cursor enter event: {}", entered); });
  }

  void view_context::post_scroll_event(GLFWwindow* win, f64 x, f64 y)
  {
    post([=](auto&) { log_info("scroll event: {}, {}", x, y); });
  }

  void view_context::post_path_drop_event(
    GLFWwindow* win,
    std::vector<std::u8string> paths)
  {
    post([=](auto&) {
      log_info("path drop event:");
      for (auto&& p : paths)
        log_info("  {}", (const char*)p.data());
    });
  }

} // namespace yave::ui