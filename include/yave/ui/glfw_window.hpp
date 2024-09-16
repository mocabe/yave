//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/glfw_context.hpp>

namespace yave::ui {

  class native_window;
  class main_context;

  class glfw_window
  {
    glfw_context& m_glfw;
    // window handle
    GLFWwindow* m_win;
    // window title
    std::u8string m_name;
    // current size
    ui::size m_size;
    // current pos
    ui::vec m_pos;
    // current framebuffer size
    ui::size m_fb_size;
    // scaling
    ui::vec m_content_scale;
    // focus
    bool m_focused = false;
    // maximize
    bool m_maximized = false;
    // minimize
    bool m_minimized = false;

  public:
    glfw_window(glfw_context& glfw, std::u8string name, ui::size size);

    ~glfw_window() noexcept;

    glfw_window(const glfw_window&) = delete;

    auto& size() const
    {
      return m_size;
    }

    auto& pos() const
    {
      return m_pos;
    }

    auto& name() const
    {
      return m_name;
    }

    auto& handle() const
    {
      return m_win;
    }

    auto& framebuffer_size() const
    {
      return m_fb_size;
    }

    auto& content_scale() const
    {
      return m_content_scale;
    }

    auto& focused() const
    {
      return m_focused;
    }

    auto& maximized() const
    {
      return m_maximized;
    }

    auto& minimized() const
    {
      return m_minimized;
    }

    void set_name(std::u8string name);

    void set_size(ui::size size);

    void set_min_size(ui::size size);

    void set_max_size(ui::size size);

    void set_pos(ui::vec pos);

    void show();

    void hide();

    void update_pos(u32 x, u32 y, passkey<native_window>)
    {
      m_pos = ui::vec(x, y);
    }

    void update_size(u32 w, u32 h, passkey<native_window>)
    {
      m_size = ui::size(w, h);
    }

    void update_framebuffer_size(u32 w, u32 h, passkey<native_window>)
    {
      m_fb_size = ui::size(w, h);
    }

    void update_content_scale(f32 xs, f32 ys, passkey<native_window>)
    {
      m_content_scale = ui::vec(xs, ys);
    }

    void update_focus(bool focused, passkey<native_window>)
    {
      m_focused = focused;
    }

    void update_maximize(bool maximized, passkey<native_window>)
    {
      m_maximized = maximized;
    }

    void update_minimize(bool minimized, passkey<native_window>)
    {
      m_minimized = minimized;
    }
  };

} // namespace yave::ui