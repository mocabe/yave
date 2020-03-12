//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/vulkan/vulkan_util.hpp>  
#include <selene/img/typed/ImageView.hpp>
#include <selene/img/pixel/PixelTypeAliases.hpp>

namespace yave::vulkan {

  // -----------------------------------------
  // single_time_command

  single_time_command::single_time_command(single_time_command&& other) noexcept
    : m_device {other.m_device}
    , m_queue {other.m_queue}
    , m_pool {other.m_pool}
    , m_buffer {std::move(other.m_buffer)}
  {
  }

  single_time_command::single_time_command(
    const vk::Device& device,
    const vk::Queue& queue,
    const vk::CommandPool& pool)
    : m_device {device}
    , m_queue {queue}
    , m_pool {pool}
  {
    // create command buffer
    {
      vk::CommandBufferAllocateInfo info {};
      info.commandPool        = m_pool;
      info.level              = vk::CommandBufferLevel::ePrimary;
      info.commandBufferCount = 1;
      m_buffer = std::move(m_device.allocateCommandBuffersUnique(info)[0]);
    }

    // fence
    {
      vk::FenceCreateInfo info {};
      m_fence = m_device.createFenceUnique(info);
    }

    // begin command buffer
    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    m_buffer->begin(beginInfo);
  }

  single_time_command::~single_time_command()
  {
    if (!m_buffer)
      return;

    // end command buffer
    m_buffer->end();
    // submit command buffer
    vk::SubmitInfo submitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &m_buffer.get();
    m_queue.submit(submitInfo, m_fence.get());
    // wait command submission
    m_device.waitForFences(
      m_fence.get(), VK_TRUE, std::numeric_limits<uint64_t>::max());
  }

  vk::CommandBuffer single_time_command::command_buffer() const
  {
    return m_buffer.get();
  }

  // -----------------------------------------
  // find memory type

  auto find_memory_type_index(
    const vk::MemoryRequirements& requirements,
    const vk::MemoryPropertyFlags& properties,
    const vk::PhysicalDevice& physicalDevice) -> uint32_t
  {
    auto props = physicalDevice.getMemoryProperties();

    for (uint32_t i = 0; i < props.memoryTypeCount; ++i) {
      auto memType = props.memoryTypes[i];
      if (requirements.memoryTypeBits & (1 << i))
        if ((memType.propertyFlags & properties) == properties)
          return i;
    }
    throw std::runtime_error("Failed to find suitable memory type");
  }

  auto format_texel_size(const vk::Format& format) -> vk::DeviceSize
  {
    // only supports format defined in image_format
    switch (format) {
      case vk::Format::eR8G8B8Unorm:
        return 3;
      case vk::Format::eR16G16B16Unorm:
        return 6;
      case vk::Format::eR8G8B8A8Unorm:
        return 4;
      case vk::Format::eR16G16B16A16Unorm:
        return 8;
      case vk::Format::eR32G32B32A32Sfloat:
        return 16;
      default:
        unreachable();
    }
  }
} // namespace yave