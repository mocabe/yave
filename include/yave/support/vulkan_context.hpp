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
    vk::Instance instance() const;
    /// Get physical device.
    vk::PhysicalDevice physical_device() const;
    /// Get device.
    vk::Device device() const;

    /* window and surface helpers */

    /// Create surface
    vk::UniqueSurfaceKHR create_window_surface(
      const std::unique_ptr<GLFWwindow, glfw_window_deleter>& window) const;

    /// Create swapchain
    vk::UniqueSwapchainKHR create_surface_swapchain(
      const vk::UniqueSurfaceKHR& surface,
      const std::unique_ptr<GLFWwindow, glfw_window_deleter>& window) const;

    /// Create image views
    std::vector<vk::UniqueImageView> create_swapchain_image_views(
      const vk::UniqueSurfaceKHR& surface,
      const vk::UniqueSwapchainKHR& swapchain) const;

    /// Get avalable surface formats
    std::vector<vk::SurfaceFormatKHR>
      get_surface_formats(const vk::SurfaceKHR& surface) const;

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
    /// index of present queue
    uint32_t m_presentQueueIndex;
    /// list of queue family indicies
    std::vector<uint32_t> m_queueFamilyIndicies;

    /* logical device */

    /// device
    vk::UniqueDevice m_device;
  };
}