//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/vulkan_device.hpp>
#include <yave/ui/glfw_context.hpp>

#include <ranges>
#include <span>

namespace {

  namespace rn = std::ranges;
  namespace rv = std::ranges::views;

  auto getGraphicsQueueIndex(const vk::PhysicalDevice& phyDev)
  {
    auto props = phyDev.getQueueFamilyProperties();

    auto it = std::find_if(props.begin(), props.end(), [&](auto&& prop) {
      return prop.queueFlags & vk::QueueFlagBits::eGraphics;
    });

    // assume vulkan_context already checked this
    assert(it != props.end());
    return static_cast<uint32_t>(std::distance(props.begin(), it));
  }

  auto getPresentQueueIndex(
    uint32_t graphicsQueue,
    const vk::Instance& inst,
    const vk::PhysicalDevice& phyDev) -> uint32_t
  {
    // when graphics queue supports presentation, use graphics queue.
    if (glfwGetPhysicalDevicePresentationSupport(inst, phyDev, graphicsQueue)) {
      return graphicsQueue;
    }

    // present queue is independent
    auto props = phyDev.getQueueFamilyProperties();
    for (uint32_t idx = 0; idx < props.size(); ++idx) {
      if (glfwGetPhysicalDevicePresentationSupport(inst, phyDev, idx)) {
        return idx;
      }
    }
    throw std::runtime_error("Presentation is not supported on this device");
  }

  bool checkDeviceExtensionSupport(
    std::span<const char*> required,
    const vk::PhysicalDevice& physDev)
  {
    auto supported = physDev.enumerateDeviceExtensionProperties();

    for (auto&& e : required) {
      auto it = rn::find_if(supported, [&](auto&& x) {
        return std::string_view(x.extensionName) == e;
      });
      if (it == supported.end())
        return false;
    }
    return true;
  }

  bool checkDeviceLayerSupport(
    std::span<const char*> required,
    const vk::PhysicalDevice& physDev)
  {
    auto supported = physDev.enumerateDeviceLayerProperties();

    for (auto&& l : required) {
      auto it = rn::find_if(supported, [&](auto&& x) {
        return std::string_view(x.layerName) == l;
      });
      if (it == supported.end())
        return false;
    }
    return true;
  }

  auto createDevice(
    const vk::PhysicalDevice& phyDev,
    uint32_t graphicsQueue,
    uint32_t presentQueue) -> vk::UniqueDevice
  {
    float queuePriority = 0.f;

    auto queueCreateInfos = [&]() -> std::vector<vk::DeviceQueueCreateInfo> {
      if (graphicsQueue != presentQueue) {
        return {
          vk::DeviceQueueCreateInfo()
            .setQueueFamilyIndex(graphicsQueue)
            .setQueueCount(1)
            .setPQueuePriorities(&queuePriority),
          vk::DeviceQueueCreateInfo()
            .setQueueFamilyIndex(presentQueue)
            .setQueueCount(1)
            .setPQueuePriorities(&queuePriority)};
      }

      return {vk::DeviceQueueCreateInfo()
                .setQueueFamilyIndex(graphicsQueue)
                .setQueueCount(1)
                .setPQueuePriorities(&queuePriority)};
    }();

    auto eNames = std::array {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME, // for swapchain
    };
    checkDeviceExtensionSupport(eNames, phyDev);

    auto lNames = std::array<const char*, 0> {};
    checkDeviceLayerSupport(lNames, phyDev);

    auto info = vk::DeviceCreateInfo()
                  .setQueueCreateInfos(queueCreateInfos)
                  .setPEnabledExtensionNames(eNames)
                  .setPEnabledLayerNames(lNames);

    return phyDev.createDeviceUnique(info);
  }

} // namespace

namespace yave::ui {

  vulkan_device::vulkan_device(
    vulkan_context& vulkan,
    glfw_context& /* only for explicit dependency */)
    : m_vulkan {vulkan}
  {
    // clang-format off

    auto inst   = vulkan.instance();
    auto phyDev = vulkan.physical_device();

    m_graphics_queue_index = getGraphicsQueueIndex(phyDev);
    m_present_queue_index  = getPresentQueueIndex(m_graphics_queue_index, inst, phyDev);

    // init 1 queue for each
    m_device         = createDevice(phyDev, m_graphics_queue_index, m_present_queue_index);
    m_graphics_queue = m_device->getQueue(m_graphics_queue_index, 0);
    m_present_queue  = m_device->getQueue(m_present_queue_index, 0);

    // clang-format on
  }

  vulkan_device::~vulkan_device() noexcept
  {
    m_device->waitIdle();
  }

} // namespace yave::ui
