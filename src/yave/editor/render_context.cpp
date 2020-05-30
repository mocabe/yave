//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/editor/render_context.hpp>

namespace yave::editor {

  class render_context::impl
  {
  public:
    impl(glfw::glfw_window& glfw_win)
      : vulkan_ctx {}
      , window_ctx {vulkan_ctx, glfw_win}
      , cmd_buff {}
    {
    }
    ~impl() noexcept = default;

    vulkan::vulkan_context vulkan_ctx;
    vulkan::window_context window_ctx;

    // in frame command buffer
    vk::CommandBuffer cmd_buff;

  public:
    void begin_frame()
    {
      window_ctx.begin_frame();
      cmd_buff = window_ctx.begin_record();
    }
    void end_frame()
    {
      window_ctx.end_record(std::exchange(cmd_buff, vk::CommandBuffer()));
      window_ctx.end_frame();
    }
  };

  render_context::render_context(glfw::glfw_window& glfw_win)
    : m_pimpl {std::make_unique<impl>(glfw_win)}
  {
  }

  render_context::~render_context() noexcept = default;

  void render_context::begin_frame()
  {
    m_pimpl->begin_frame();
  }
  void render_context::end_frame()
  {
    m_pimpl->end_frame();
  }

  auto render_context::vulkan_context() const -> const vulkan::vulkan_context&
  {
    return m_pimpl->vulkan_ctx;
  }

  auto render_context::vulkan_context() -> vulkan::vulkan_context&
  {
    return m_pimpl->vulkan_ctx;
  }

  auto render_context::vulkan_window() const -> const vulkan::window_context&
  {
    return m_pimpl->window_ctx;
  }

  auto render_context::vulkan_window() -> vulkan::window_context&
  {
    return m_pimpl->window_ctx;
  }

} // namespace yave::editor