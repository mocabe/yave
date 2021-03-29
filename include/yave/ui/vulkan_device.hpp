//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/vulkan_context.hpp>

namespace yave::ui {

  class glfw_context;

  /// Logical device for view
  class vulkan_device
  {
    vulkan_context& m_vulkan;
    vk::UniqueDevice m_device;

  private:
    uint32_t m_graphics_queue_index;
    uint32_t m_present_queue_index;
    vk::Queue m_graphics_queue;
    vk::Queue m_present_queue;

  public:
    vulkan_device(vulkan_context& vulkan, glfw_context& glfw);
    ~vulkan_device() noexcept;

    auto instance() const
    {
      return m_vulkan.instance();
    }

    auto physical_device() const
    {
      return m_vulkan.physical_device();
    }

    auto device() const
    {
      return m_device.get();
    }

    void wait_idle()
    {
      return m_device->waitIdle();
    }

  public:
    auto graphics_queue_index() const
    {
      return m_graphics_queue_index;
    }

    auto present_queue_index() const
    {
      return m_present_queue_index;
    }

    auto graphics_queue() const
    {
      return m_graphics_queue;
    }

    auto present_queue() const
    {
      return m_present_queue;
    }

  };

} // namespace yave::ui