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
    // main
    main_context& m_mctx;
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

    auto& fb_size() const
    {
      return m_fb_size;
    }

    void set_name(std::u8string name);

    void set_size(ui::size size);

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

    void update_fb_size(u32 w, u32 h, passkey<native_window>)
    {
      m_fb_size = ui::size(w, h);
    }
  };

} // namespace yave::ui