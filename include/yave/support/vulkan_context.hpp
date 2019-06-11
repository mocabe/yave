//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/support/glfw_context.hpp>
#include <vulkan/vulkan.hpp>

namespace yave {

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

    public:
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
      [[nodiscard]] vk::Fence fence() const;
      [[nodiscard]] vk::Semaphore image_acquired_semaphore() const;
      [[nodiscard]] vk::Semaphore render_complete_semaphore() const;
      [[nodiscard]] vk::PipelineCache pipeline_cache() const;

    public:
      [[nodiscard]] GLFWwindow* window() const;
      [[nodiscard]] bool resized() const;
      [[nodiscard]] bool should_close() const;

    public:
      /// rebuild resources related to frame buffer.
      /// \note: Not internally synchronized.
      void rebuild_swapchain();

    private:
      class impl;
      std::unique_ptr<impl> m_pimpl;
    };

    /// Create new window context
    vulkan_context::window_context create_window_context(
      std::unique_ptr<GLFWwindow, glfw_window_deleter>& window) const;

  private:
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