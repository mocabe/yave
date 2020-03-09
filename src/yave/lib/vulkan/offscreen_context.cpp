//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/vulkan/offscreen_context.hpp>

namespace {
  using namespace yave::vulkan;

  auto getGraphicsQueueIndex(const vk::PhysicalDevice& physicalDevice)
    -> uint32_t
  {
    auto properties = physicalDevice.getQueueFamilyProperties();

    for (uint32_t i = 0; i < properties.size(); ++i) {
      if (properties[i].queueFlags & vk::QueueFlagBits::eGraphics)
        return i;
    }

    throw std::runtime_error("Device does not support graphics queue");
  }

  auto getDeviceExtensions() -> std::vector<std::string>
  {
    return {};
  }

  auto getDeviceLayers() -> std::vector<std::string>
  {
    return {};
  }

  auto getDeviceQueue(uint32_t queueFamilyIndex, const vk::Device& device)
    -> vk::Queue
  {
    // Assume only single queue is initialized.
    return device.getQueue(queueFamilyIndex, 0);
  }
} // namespace

namespace yave::vulkan {

  class offscreen_context::impl
  {
  public:
    vulkan_context& vulkan_ctx;

  public:
    vk::UniqueDevice device;

  public:
    uint32_t graphics_queue_index;
    vk::Queue graphics_queue;

  public:
    impl(vulkan_context& ctx)
      : vulkan_ctx {ctx}
    {
      graphics_queue_index = getGraphicsQueueIndex(ctx.physical_device());

      device = ctx.create_device(
        {graphics_queue_index}, getDeviceExtensions(), getDeviceLayers());

      graphics_queue = getDeviceQueue(graphics_queue_index, device.get());
    }

    ~impl() noexcept
    {
      device->waitIdle();
    }
  };

  offscreen_context::offscreen_context(vulkan_context& ctx)
    : m_pimpl {std::make_unique<impl>(ctx)}
  {
  }

  offscreen_context::~offscreen_context() noexcept
  {
  }

  auto offscreen_context::vulkan_ctx() -> vulkan_context&
  {
    return m_pimpl->vulkan_ctx;
  }

  auto offscreen_context::device() const -> vk::Device
  {
    return m_pimpl->device.get();
  }

  auto offscreen_context::graphics_queue_index() const -> uint32_t
  {
    return m_pimpl->graphics_queue_index;
  }

  auto offscreen_context::graphics_queue() const -> vk::Queue
  {
    return m_pimpl->graphics_queue;
  }
}