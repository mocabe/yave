//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/event.hpp>
#include <yave/ui/typedefs.hpp>
#include <yave/ui/key_events.hpp>
#include <yave/ui/mouse_events.hpp>

#include <memory>
#include <GLFW/glfw3.h>

namespace yave::ui {

  class view_context;
  class data_context;
  class window_manager;

  /// Dispatch window events
  class window_event_dispatcher
  {
    class impl;
    std::unique_ptr<impl> m_pimpl;

  public:
    window_event_dispatcher(window_manager&, view_context&);
    ~window_event_dispatcher() noexcept;

  public:
    bool has_pending_events() const;
    void dispatch_pending_events();

  public:
    void process_show_event(window* w);
    void process_hide_event(window* w);

  public:
    void push_pos_event(GLFWwindow* win, u32 x, u32 y);
    void push_size_event(GLFWwindow* win, u32 w, u32 h);
    void push_close_event(GLFWwindow* win);
    void push_refresh_event(GLFWwindow* win);
    void push_focus_event(GLFWwindow* win, bool focused);
    void push_fb_size_event(GLFWwindow* win, u32 w, u32 h);
    void push_content_scale_event(GLFWwindow* win, f32 xs, f32 ys);
    void push_minimize_event(GLFWwindow* win, bool minimized);
    void push_maximize_event(GLFWwindow* win, bool maximized);

  public:
    // clang-format off
    void push_key_event(GLFWwindow* win, ui::key key, ui::key_action action,ui::key_modifier_flags mods);
    void push_char_event(GLFWwindow* win, u32 codepoint);
    void push_mouse_event(GLFWwindow* win, ui::mouse_button button, ui::mouse_button_action action, ui::key_modifier_flags mods);
    void push_cursor_pos_event(GLFWwindow* win, f64 x, f64 y);
    void push_cursor_enter_event(GLFWwindow* win, bool entered);
    void push_scroll_event(GLFWwindow* win, f64 x, f64 y);
    void push_path_drop_event(GLFWwindow* win, std::vector<std::u8string> paths);
    // clang-format on
  };

} // namespace yave::ui