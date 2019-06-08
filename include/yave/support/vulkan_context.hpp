//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <vulkan/vulkan.hpp>

namespace yave {

  /// Vulkan API context.
  class vulkan_context
  {
  public:
    /// Ctor.
    vulkan_context(bool enable_validation_layer = true);
    /// Dtor.
    ~vulkan_context();

    /// Get instance.
    vk::Instance instance() const;
    /// Get physical device.
    vk::PhysicalDevice physical_device() const;
    /// Get device.
    vk::Device device() const;

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

    /// list of queues
    std::vector<vk::QueueFlagBits> m_queueFlags;
    /// list of queue family indicies
    std::vector<uint32_t> m_queueFamilyIndicies;
    /// index of present queue
    uint32_t m_presentQueueIndex;

    /* logical device */

    /// device
    vk::UniqueDevice m_device;
  };
}