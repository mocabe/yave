//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/vulkan/vulkan_context.hpp>

namespace yave::vulkan {

  /// glfw/vulkan window context
  class window_context
  {
  public:
    /// Ctor
    window_context(vulkan_context& ctx, glfw::glfw_window& win);
    /// Move window context.
    window_context(window_context&& other) noexcept;
    /// Dtor
    ~window_context() noexcept;

  public: /* data access */
    /// Get surface
    [[nodiscard]] auto surface() const -> vk::SurfaceKHR;
    /// Get swapchain
    [[nodiscard]] auto swapchain() const -> vk::SwapchainKHR;
    /// Get swapchain images
    [[nodiscard]] auto swapchain_images() const -> std::vector<vk::Image>;
    /// Get swapchain image views
    [[nodiscard]] auto swapchain_image_views() const
      -> std::vector<vk::ImageView>;
    /// Get frame buffers
    [[nodiscard]] auto frame_buffers() const -> std::vector<vk::Framebuffer>;
    /// Get current swapchain format
    [[nodiscard]] auto swapchain_format() const -> vk::SurfaceFormatKHR;
    /// Get current swapchain extent
    [[nodiscard]] auto swapchain_extent() const -> vk::Extent2D;
    /// Get render pass
    [[nodiscard]] auto render_pass() const -> vk::RenderPass;
    /// Get command pool
    [[nodiscard]] auto command_pool() const -> vk::CommandPool;
    /// Get command buffers
    [[nodiscard]] auto command_buffers() const
      -> std::vector<vk::CommandBuffer>;

  public: /* window state */
    /// Get window
    [[nodiscard]] auto window() const -> GLFWwindow*;
    /// Check if frame buffer is resized.
    [[nodiscard]] bool resized() const;
    /// Check if widnow should close.
    [[nodiscard]] bool should_close() const;

  public: /* window settings */
    /// Set clear color
    void set_clear_color(float r, float g, float b, float a);

  public: /* framebuffer update */
    /// Rebuild resources related to frame buffer.
    /// \note: Not internally synchronized.
    /// \note: Does not poll glfw event. Use resized() to check resize event.
    /// \note: This function will block rendering thread when frame buffer
    /// size is zero. Happens when window is minimized on Windows platform.
    void rebuild_frame_buffers();

  public: /* render operations */
    /// Update frame/semaphore indicies and acquire new image.
    void begin_frame();

    /// Submit current frame buffer.
    void end_frame();

    /// Begin recording command.
    /// \note should be called between begin_frame() and end_frame()
    [[nodiscard]] auto begin_record() -> vk::CommandBuffer;

    /// End recording command.
    /// \note should be called between begin_frame() and end_frame()
    void end_record(const vk::CommandBuffer& buffer);

    /// Get current swapchain index.
    /// \note: Swapchain index is given by driver every frame, and there's no
    /// guarantee about order of index. This index should only be used for
    /// specifying swapchain resources like image or frame buffer.
    /// \note: Maximum value of swapchain index is swapchain_image_count()-1
    /// or swapchain_index_count()-1;
    [[nodiscard]] auto swapchain_index() const -> uint32_t;

    /// Get number of swapchain index.
    /// \returns swapchain_image_count()
    [[nodiscard]] auto swapchain_index_count() const -> uint32_t;

    /// Get current frame index. This index can be used for resources for each
    /// render operation.
    [[nodiscard]] auto frame_index() const -> uint32_t;

    /// Get number of frame index. This value also represents maximum number
    /// of in-flight render operations.
    [[nodiscard]] auto frame_index_count() const -> uint32_t;

  public:
    /// Create single time command
    [[nodiscard]] auto single_time_command() const
      -> vulkan::single_time_command;

  private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
  };
}