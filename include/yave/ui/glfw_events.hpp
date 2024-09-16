//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/typedefs.hpp>
#include <yave/ui/glfw_context.hpp>
#include <yave/ui/key_events.hpp>
#include <yave/ui/mouse_events.hpp>

#include <vector>
#include <variant>

namespace yave::ui {

  struct glfw_pos_event
  {
    GLFWwindow* win;
    u32 x, y;
  };

  struct glfw_size_event
  {
    GLFWwindow* win;
    u32 w, h;
  };

  struct glfw_close_event
  {
    GLFWwindow* win;
  };

  struct glfw_refresh_event
  {
    GLFWwindow* win;
  };

  struct glfw_focus_event
  {
    GLFWwindow* win;
    bool focused;
  };

  struct glfw_minimize_event
  {
    GLFWwindow* win;
    bool minimized;
  };

  struct glfw_maximize_event
  {
    GLFWwindow* win;
    bool maximized;
  };

  struct glfw_framebuffer_size_event
  {
    GLFWwindow* win;
    u32 w, h;
  };

  struct glfw_content_scale_event
  {
    GLFWwindow* win;
    f32 xs, ys;
  };

  struct glfw_key_event
  {
    GLFWwindow* win;
    ui::key key;
    ui::key_action action;
    ui::key_modifiers mods;
  };

  struct glfw_char_event
  {
    GLFWwindow* win;
    u32 codepoint;
  };

  struct glfw_button_event
  {
    GLFWwindow* win;
    ui::mouse_button button;
    ui::button_action action;
    ui::key_modifiers mods;
  };

  struct glfw_cursor_pos_event
  {
    GLFWwindow* win;
    f64 xpos, ypos;
  };

  struct glfw_cursor_enter_event
  {
    GLFWwindow* win;
    bool entered;
  };

  struct glfw_scroll_event
  {
    GLFWwindow* win;
    f64 xoffset, yoffset;
  };

  struct glfw_path_drop_event
  {
    GLFWwindow* win;
    std::vector<std::u8string> paths;
  };

  /// GLFW event type
  using glfw_event = std::variant<
    glfw_pos_event,
    glfw_size_event,
    glfw_close_event,
    glfw_refresh_event,
    glfw_focus_event,
    glfw_minimize_event,
    glfw_maximize_event,
    glfw_framebuffer_size_event,
    glfw_content_scale_event,
    glfw_key_event,
    glfw_char_event,
    glfw_button_event,
    glfw_cursor_pos_event,
    glfw_cursor_enter_event,
    glfw_scroll_event,
    glfw_path_drop_event>;

} // namespace yave::ui
