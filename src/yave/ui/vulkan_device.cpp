//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/vulkan_device.hpp>
#include <yave/ui/glfw_context.hpp>
#include <yave/ui/draw_list.hpp> 
#include <yave/ui/typedefs.hpp>

#include <ranges>
#include <span>

namespace {

  using namespace yave::ui;
  namespace rn = std::ranges;

  auto getGraphicsQueueFamily(const vk::PhysicalDevice& phyDev)
  {
    auto props = phyDev.getQueueFamilyProperties();

    auto it = std::find_if(props.begin(), props.end(), [&](auto&& prop) {
      return prop.queueFlags & vk::QueueFlagBits::eGraphics;
    });

    // assume vulkan_context already checked this
    assert(it != props.end());
    return static_cast<u32>(std::distance(props.begin(), it));
  }

  auto getPresentQueueFamily(
    u32 graphicsQueueFamily,
    const vk::Instance& inst,
    const vk::PhysicalDevice& phyDev) -> u32
  {
    // when graphics queue supports presentation, use graphics queue.
    if (glfwGetPhysicalDevicePresentationSupport(
          inst, phyDev, graphicsQueueFamily)) {
      return graphicsQueueFamily;
    }

    // present queue is independent
    auto props = phyDev.getQueueFamilyProperties();
    for (u32 idx = 0; idx < props.size(); ++idx) {
      if (glfwGetPhysicalDevicePresentationSupport(inst, phyDev, idx)) {
        return idx;
      }
    }
    throw std::runtime_error("Presentation is not supported on this device");
  }

  auto getTransferQueueFamily(
    u32 graphicsQueueFamily,
    const vk::PhysicalDevice& physDev) -> u32
  {
    auto props       = physDev.getQueueFamilyProperties();
    auto transferBit = vk::QueueFlagBits::eTransfer;
    auto graphicsBit = vk::QueueFlagBits::eGraphics;
    auto computeBit  = vk::QueueFlagBits::eCompute;

    // find dedicated transfer queue
    for (u32 idx = 0; idx < props.size(); ++idx) {
      auto flags = props[idx].queueFlags;
      if ((flags & transferBit) && !(flags & (graphicsBit | computeBit)))
        return idx;
    }

    // when graphics queue supports transfer, use it
    if (props[graphicsQueueFamily].queueFlags & transferBit)
      return graphicsQueueFamily;

    // find any queue which supports transfer
    for (u32 idx = 0; idx < props.size(); ++idx) {
      auto flags = props[idx].queueFlags;
      if (flags & transferBit)
        return idx;
    }
    throw std::runtime_error("Transfer is not supported on this device");
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
    u32 graphicsQueueFamily,
    u32 transferQueueFamily,
    u32 presentQueueFamily) -> vk::UniqueDevice
  {
    float queuePriority = 0.f;

    auto queueFamilies =
      std::array {graphicsQueueFamily, transferQueueFamily, presentQueueFamily};

    std::sort(queueFamilies.begin(), queueFamilies.end());
    auto end = std::unique(queueFamilies.begin(), queueFamilies.end());

    auto queueCreateInfos = std::vector<vk::DeviceQueueCreateInfo>();
    queueCreateInfos.reserve(std::distance(queueFamilies.begin(), end));

    for (auto it = queueFamilies.begin(); it != end; ++it) {
      queueCreateInfos.push_back(vk::DeviceQueueCreateInfo()
                                   .setQueueFamilyIndex(*it)
                                   .setQueueCount(1)
                                   .setPQueuePriorities(&queuePriority));
    }

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

  auto createCommandPool(u32 queueFamily, const vk::Device& device)
    -> vk::UniqueCommandPool
  {
    auto info = vk::CommandPoolCreateInfo()
                  // allow vkResetCommandBuffer
                  .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                  // use graphics queue
                  .setQueueFamilyIndex(queueFamily);

    return device.createCommandPoolUnique(info);
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

    m_graphics_queue_family = getGraphicsQueueFamily(phyDev);
    m_transfer_queue_family = getTransferQueueFamily(m_graphics_queue_family,phyDev);
    m_present_queue_family  = getPresentQueueFamily(m_graphics_queue_family, inst, phyDev);

    // init 1 queue for each
    m_device         = createDevice(phyDev, m_graphics_queue_family, m_transfer_queue_family, m_present_queue_family);
    m_graphics_queue = m_device->getQueue(m_graphics_queue_family, 0);
    m_transfer_queue = m_device->getQueue(m_transfer_queue_family, 0);
    m_present_queue  = m_device->getQueue(m_present_queue_family, 0);

    // init command queue
    m_graphics_command_pool = createCommandPool(m_graphics_queue_family, m_device.get());

    if (m_graphics_queue_family != m_transfer_queue_family)
      m_transfer_command_pool = createCommandPool(m_transfer_queue_family, m_device.get());

    // clang-format on
  }

  vulkan_device::~vulkan_device() noexcept
  {
    m_device->waitIdle();
  }

} // namespace yave::ui
