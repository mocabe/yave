//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/command.hpp>
#include <yave/ui/data_context.hpp>
#include <yave/ui/main_context.hpp>
#include <yave/ui/window_manager.hpp>
#include <yave/ui/key_events.hpp>
#include <yave/ui/mouse_events.hpp>

#include <memory>

namespace yave::ui {

  class view_context;
  class layout_context;
  class render_context;

  /// View context command
  using view_command = command<view_context>;

  /// View context
  class view_context
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    /// Init view context
    view_context(main_context& mctx, data_context& dctx);
    /// Deinit view context
    ~view_context() noexcept;

  public:
    /// data
    auto data_ctx() -> data_context&;
    auto data_ctx() const -> const data_context&;
    /// main
    auto main_ctx() -> main_context&;
    auto main_ctx() const -> const main_context&;
    /// layout
    auto layout_ctx() -> ui::layout_context&;
    auto layout_ctx() const -> const ui::layout_context&;
    /// render
    auto render_ctx() -> ui::render_context&;
    auto render_ctx() const -> const ui::render_context&;

  private:
    void _post(view_command op) const;
    void _post_delay(view_command op) const;
    void _post_empty() const;

  public:
    /// Post view command
    template <class F>
    void post(F&& f)
    {
      _post(view_command(std::forward<F>(f)));
    }

    /// Post delayed view command
    /// \note This function is mainly designed to be called from main loop so we
    /// can guarantee execution of delayed commands are postponed until the
    /// beginning of next loop. You can still call this function from other
    /// threads but there's no guarantee about when posted commands are
    /// executed. Use task threads or timers for better control of delays.
    template <class F>
    void post_delay(F&& f)
    {
      _post_delay(view_command(std::forward<F>(f)));
    }

    /// Post empty command to wake view thread
    void wake()
    {
      _post_empty();
    }

  public:
    /// Start event loop.
    /// This function will not return until view thread exit.
    void run();
    /// Exit view thread.
    void exit();

  public:
    /// get window manager
    auto window_manager() const -> const ui::window_manager&;
    auto window_manager() -> ui::window_manager&;

  public:
    // clang-format off

    /// window events
    void post_window_pos_event(GLFWwindow* win, u32 x, u32 y);
    void post_window_size_event(GLFWwindow* win, u32 w, u32 h);
    void post_window_close_event(GLFWwindow* win);
    void post_window_refresh_event(GLFWwindow* win);
    void post_window_focus_event(GLFWwindow* win, bool focused);
    void post_window_minimize_event(GLFWwindow* win, bool minimized);
    void post_window_maximize_event(GLFWwindow* win, bool maximized);
    void post_window_framebuffer_size_event(GLFWwindow* win, u32 w, u32 h);
    void post_window_content_scale_event(GLFWwindow* win, f32 xs, f32 ys);

    /// input events
    void post_key_event(GLFWwindow* win, ui::key key, ui::key_action action, ui::key_modifier_flags mods);
    void post_char_event(GLFWwindow* win, u32 codepoint);
    void post_mouse_event(GLFWwindow* win, ui::mouse_button button, ui::mouse_button_action action, ui::key_modifier_flags mods);
    void post_cursor_pos_event(GLFWwindow* win, f64 x, f64 y);
    void post_cursor_enter_event(GLFWwindow* win, bool entered);
    void post_scroll_event(GLFWwindow* win, f64 x, f64 y);
    void post_path_drop_event(GLFWwindow* win, std::vector<std::u8string> paths);

    // clang-format on
  };

} // namespace yave::ui