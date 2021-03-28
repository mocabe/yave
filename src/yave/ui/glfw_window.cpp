//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/glfw_window.hpp>
#include <yave/ui/main_context.hpp>

namespace yave::ui {

  glfw_window::glfw_window(
    glfw_context& glfw,
    std::u8string name,
    ui::size size)
    : m_glfw {glfw}
    , m_mctx {glfw.main_ctx()}
    , m_name {std::move(name)}
  {
    m_mctx
      .sync_post([&](auto&) {
        auto& glfw = m_mctx.glfw_ctx();
        m_win      = glfw.create_window(m_name, size);
        m_size     = glfw.window_size(m_win);
        m_pos      = glfw.window_pos(m_win);
      })
      .wait();
  }

  glfw_window::~glfw_window() noexcept
  {
    m_mctx.sync_post([&](auto&) { m_mctx.glfw_ctx().destroy_window(m_win); })
      .wait();
  }

  void glfw_window::set_name(std::u8string name)
  {
    m_name = std::move(name);
    m_mctx
      .sync_post([&](auto&) { m_mctx.glfw_ctx().rename_window(m_win, m_name); })
      .wait();
  }

  void glfw_window::set_size(ui::size size)
  {
    m_mctx
      .sync_post([&](auto&) { m_mctx.glfw_ctx().resize_window(m_win, size); })
      .wait();
  }

  void glfw_window::set_pos(ui::vec pos)
  {
    m_mctx.sync_post([&](auto&) { m_mctx.glfw_ctx().move_window(m_win, pos); })
      .wait();
  }

  void glfw_window::show()
  {
    m_mctx.sync_post([&](auto&) { m_mctx.glfw_ctx().show_window(m_win); })
      .wait();
  }

  void glfw_window::hide()
  {
    m_mctx.sync_post([&](auto&) { m_mctx.glfw_ctx().hide_window(m_win); })
      .wait();
  }

} // namespace yave::ui
