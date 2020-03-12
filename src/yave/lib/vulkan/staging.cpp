//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/vulkan/staging.hpp>

namespace yave::vulkan {

  auto create_staging_buffer(
    const vk::DeviceSize& size,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice) -> staging_buffer
  {
    vk::UniqueBuffer buffer;
    {
      vk::BufferCreateInfo info;
      info.size  = size;
      info.usage = vk::BufferUsageFlagBits::eTransferDst
                   | vk::BufferUsageFlagBits::eTransferSrc;
      info.sharingMode = vk::SharingMode::eExclusive;

      buffer = device.createBufferUnique(info);
    }

    vk::UniqueDeviceMemory bufferMemory;
    {
      auto memReq = device.getBufferMemoryRequirements(buffer.get());

      vk::MemoryAllocateInfo info;
      info.allocationSize  = memReq.size;
      info.memoryTypeIndex = find_memory_type_index(
        memReq,
        vk::MemoryPropertyFlagBits::eHostVisible
          | vk::MemoryPropertyFlagBits::eHostCached,
        physicalDevice);

      bufferMemory = device.allocateMemoryUnique(info);
      device.bindBufferMemory(buffer.get(), bufferMemory.get(), 0);
    }

    return {std::move(buffer), std::move(bufferMemory), size};
  }

  void resize_staging_buffer(
    staging_buffer& staging,
    const vk::DeviceSize& size,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice)
  {
    assert(size != 0);
    assert(staging.buffer);

    if (staging.size >= size)
      return;

    auto tmp = create_staging_buffer(size, device, physicalDevice);
    std::swap(staging, tmp);
  }
} // namespace yave::vulkan