//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/glfw_window.hpp>

namespace yave::ui {

  glfw_window::glfw_window(
    glfw_context& glfw,
    std::u8string name,
    ui::size size)
    : m_glfw {glfw}
    , m_name {std::move(name)}
  {
    m_win           = m_glfw.create_window(m_name, size);
    m_size          = m_glfw.window_size(m_win);
    m_pos           = m_glfw.window_pos(m_win);
    m_content_scale = m_glfw.window_content_scale(m_win);
  }

  glfw_window::~glfw_window() noexcept
  {
    m_glfw.destroy_window(m_win);
  }

  void glfw_window::set_name(std::u8string name)
  {
    m_glfw.rename_window(m_win, name);
    m_name = std::move(name);
  }

  void glfw_window::set_size(ui::size size)
  {
    m_glfw.resize_window(m_win, size);
  }

  void glfw_window::set_min_size(ui::size size)
  {
    m_glfw.set_window_min_size(m_win, size);
  }

  void glfw_window::set_max_size(ui::size size)
  {
    m_glfw.set_window_max_size(m_win, size);
  }

  void glfw_window::set_pos(ui::vec pos)
  {
    m_glfw.move_window(m_win, pos);
  }

  void glfw_window::show()
  {
    m_glfw.show_window(m_win);
  }

  void glfw_window::hide()
  {
    m_glfw.hide_window(m_win);
  }

} // namespace yave::ui
