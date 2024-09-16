//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/vulkan/vulkan_util.hpp>

namespace yave::ui {

  /// Vulkan context
  /// TODO: move this into yave::vulkan namespace
  class vulkan_context
  {
    /// instance
    vk::UniqueInstance m_instance;
    /// validation callback
    vk::UniqueDebugReportCallbackEXT m_debugReportCallback;
    /// default physical device
    vk::PhysicalDevice m_physicalDevice;

  public:
    /// Ctor.
    vulkan_context();
    /// Deleted
    vulkan_context(const vulkan_context&) = delete;

  public: /* instance, devices */
          /// Get instance.
    auto instance() const
    {
      return m_instance.get();
    }

    /// Get physical device.
    auto physical_device() const
    {
      return m_physicalDevice;
    }
  };

} // namespace yave::ui