//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/support/glfw_context.hpp>
#include <vulkan/vulkan.hpp>

namespace yave {

  /// single time command
  class single_time_command
  {
  public:
    single_time_command(
      const vk::Device& device,
      const vk::Queue& queue,
      const vk::CommandPool& pool);
    ~single_time_command();
    single_time_command(single_time_command&& other) noexcept;
    [[nodiscard]] vk::CommandBuffer command_buffer() const;

  private:
    single_time_command(const single_time_command&) = delete;
    vk::Device m_device;
    vk::Queue m_queue;
    vk::CommandPool m_pool;
    vk::UniqueCommandBuffer m_buffer;
    vk::UniqueFence m_fence;
  };

  /// Vulkan API context.
  class vulkan_context
  {
  public:
    /// Ctor.
    vulkan_context(glfw_context& glfw_ctx, bool enable_validation_layer = true);
    /// Dtor.
    ~vulkan_context() noexcept;

    /* instance, devices */

    /// Get instance.
    [[nodiscard]] vk::Instance instance() const;
    /// Get physical device.
    [[nodiscard]] vk::PhysicalDevice physical_device() const;
    /// Get device.
    [[nodiscard]] vk::Device device() const;
    /// Get graphics queue
    [[nodiscard]] vk::Queue graphics_queue() const;
    /// Get present queue
    [[nodiscard]] vk::Queue present_queue() const;

    /* window and surface helpers */

    /// glfw/vulkan window context
    class window_context
    {
      friend class vulkan_context;
      window_context();

    public: /* data access */
      window_context(window_context&& other) noexcept;
      ~window_context() noexcept;
      [[nodiscard]] vk::SurfaceKHR surface() const;
      [[nodiscard]] vk::SwapchainKHR swapchain() const;
      [[nodiscard]] std::vector<vk::Image> swapchain_images() const;
      [[nodiscard]] std::vector<vk::ImageView> swapchain_image_views() const;
      [[nodiscard]] std::vector<vk::Framebuffer> frame_buffers() const;
      [[nodiscard]] vk::SurfaceFormatKHR swapchain_format() const;
      [[nodiscard]] vk::Extent2D swapchain_extent() const;
      [[nodiscard]] vk::RenderPass render_pass() const;
      [[nodiscard]] vk::CommandPool command_pool() const;
      [[nodiscard]] std::vector<vk::CommandBuffer> command_buffers() const;
      [[nodiscard]] std::vector<vk::Semaphore> acquire_semaphores() const;
      [[nodiscard]] std::vector<vk::Semaphore> complete_semaphores() const;

    public: /* window state */
      [[nodiscard]] GLFWwindow* window() const;
      [[nodiscard]] bool resized() const;
      [[nodiscard]] bool should_close() const;

    public: /* framebuffer update */
      /// rebuild resources related to frame buffer.
      /// \note: Not internally synchronized.
      void rebuild_frame_buffers() const;

    private: /* render operations */
      /// update frame/semaphore indicies and acquire new image
      vk::CommandBuffer begin_frame() const;
      /// submit current frame buffer
      void end_frame() const;

    public:
      /// RAII frame context
      class command_recorder
      {
      public:
        command_recorder(command_recorder&& other) noexcept;
        ~command_recorder();
        [[nodiscard]] vk::CommandBuffer command_buffer() const;

      private:
        friend class window_context;
        command_recorder(const command_recorder&) = delete;
        command_recorder(const window_context* window_ctx);
        const window_context* m_window_ctx;
        vk::CommandBuffer m_buffer;
      };

      /// create frame context
      [[nodiscard]] command_recorder new_frame() const;

    public:
      /// Create single time command
      [[nodiscard]] yave::single_time_command single_time_command() const;

    private:
      class impl;
      std::unique_ptr<impl> m_pimpl;
    };

    /// Create new window context
    vulkan_context::window_context
      create_window_context(unique_glfw_window& window) const;

  private:
    // glfw
    glfw_context* m_glfw;

    /* instance */

    /// instance
    vk::UniqueInstance m_instance;
    /// validation callback
    vk::UniqueDebugReportCallbackEXT m_debugCallback;

    /* physical device */

    /// physical device
    vk::PhysicalDevice m_physicalDevice;
    /// property of physical device
    vk::PhysicalDeviceProperties m_physicalDeviceProperties;
    /// queue family property of physical device
    std::vector<vk::QueueFamilyProperties>
      m_physicalDeviceQueueFamilyProperties;

    /* device queue */

    /// index of graphics queue
    uint32_t m_graphicsQueueIndex;
    /// graphics queue
    vk::Queue m_graphicsQueue;
    /// index of present queue
    uint32_t m_presentQueueIndex;
    /// present queue
    vk::Queue m_presentQueue;

    /* logical device */

    /// device
    vk::UniqueDevice m_device;
  };
}