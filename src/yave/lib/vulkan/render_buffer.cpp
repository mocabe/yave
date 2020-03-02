//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/vulkan/render_buffer.hpp>

namespace yave::vulkan {

  auto create_render_buffer(
    const vk::BufferUsageFlagBits& usg,
    const vk::DeviceSize& size,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice) -> render_buffer
  {
    // aoivd zero-sized buffer, which is invalid
    auto trueSize = size ? size : 1;

    vk::UniqueBuffer buffer;
    {
      vk::BufferCreateInfo info;
      info.size        = trueSize;
      info.usage       = usg;
      info.sharingMode = vk::SharingMode::eExclusive;

      buffer = device.createBufferUnique(info);
    }

    vk::UniqueDeviceMemory memory;
    {
      auto req = device.getBufferMemoryRequirements(buffer.get());

      vk::MemoryAllocateInfo info;
      info.allocationSize  = req.size;
      info.memoryTypeIndex = find_memory_type_index(
        req, vk::MemoryPropertyFlagBits::eHostVisible, physicalDevice);

      memory = device.allocateMemoryUnique(info);
      device.bindBufferMemory(buffer.get(), memory.get(), 0);
    }

    return {usg, size, trueSize, std::move(buffer), std::move(memory)};
  }

  void resize_render_buffer(
    render_buffer& buff,
    const vk::DeviceSize& newSize,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice)
  {
    if (buff.usage == vk::BufferUsageFlags())
      throw std::runtime_error("Render buffer is not initialized");

    auto trueSize = newSize ? newSize : 1;

    // new buffer
    vk::UniqueBuffer buffer;
    {
      vk::BufferCreateInfo info;
      info.size        = trueSize;
      info.usage       = buff.usage;
      info.sharingMode = vk::SharingMode::eExclusive;

      buffer = device.createBufferUnique(info);
    }

    // reuse memory when in capacity
    if (newSize <= buff.capacity) {
      device.bindBufferMemory(buffer.get(), buff.memory.get(), 0);
      buff.buffer = std::move(buffer);
      buff.size   = newSize;
      return;
    }

    vk::UniqueDeviceMemory memory;
    {
      auto req = device.getBufferMemoryRequirements(buffer.get());

      vk::MemoryAllocateInfo info;
      info.allocationSize  = req.size;
      info.memoryTypeIndex = find_memory_type_index(
        req, vk::MemoryPropertyFlagBits::eHostVisible, physicalDevice);

      memory = device.allocateMemoryUnique(info);
      device.bindBufferMemory(buffer.get(), memory.get(), 0);
    }

    buff.size     = newSize;
    buff.capacity = newSize;
    buff.memory   = std::move(memory);
    buff.buffer   = std::move(buffer);
  }

  void store_render_buffer(
    render_buffer& dst,
    const std::byte* srcData,
    const vk::DeviceSize& srcOffset,
    const vk::DeviceSize& srcSize,
    const vk::Device& device,
    const vk::PhysicalDevice&)
  {
    assert(dst.size >= srcOffset + srcSize);
    void* ptr = device.mapMemory(dst.memory.get(), srcOffset, srcSize);
    std::memcpy(ptr, srcData, srcSize);

    // flush since buffers are non-coherent
    vk::MappedMemoryRange range;
    range.memory = dst.memory.get();
    device.flushMappedMemoryRanges(range);

    device.unmapMemory(dst.memory.get());
  }
}