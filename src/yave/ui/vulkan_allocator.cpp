//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/vulkan_allocator.hpp>
#include <yave/ui/typedefs.hpp>

using namespace yave::ui;

namespace {

  auto createVmaAllocator(
    vk::Instance instance,
    vk::PhysicalDevice physicalDevice,
    vk::Device device)
  {
    VmaAllocatorCreateInfo info {};
    info.vulkanApiVersion = VK_VERSION_1_2;
    info.instance         = instance;
    info.physicalDevice   = physicalDevice;
    info.device           = device;

    VmaAllocator allocator;
    vmaCreateAllocator(&info, &allocator);
    return allocator;
  }

} // namespace

namespace yave::ui {

  vulkan_allocator::vulkan_allocator(vulkan_device& device)
    : m_device {device}
    , m_allocator(createVmaAllocator(
        device.instance(),
        device.physical_device(),
        device.device()))
  {
  }

  vulkan_allocator::~vulkan_allocator() noexcept
  {
    vmaDestroyAllocator(m_allocator);
  }

  auto vulkan_allocator::create_buffer(
    vk::BufferCreateInfo info,
    VmaMemoryUsage usage) -> vulkan_buffer
  {
    VkBufferCreateInfo bufferCreateInfo = info;

    VmaAllocationCreateInfo allocCreateInfo {};
    allocCreateInfo.usage = usage;
    allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    vulkan_buffer buffer {};
    VmaAllocationInfo allocInfo {};

    vmaCreateBuffer(
      m_allocator,
      &bufferCreateInfo,
      &allocCreateInfo,
      &buffer.m_buffer,
      &buffer.m_allocation,
      &allocInfo);

    vmaGetMemoryTypeProperties(
      m_allocator, allocInfo.memoryType, &buffer.m_flags);

    buffer.m_allocator   = m_allocator;
    buffer.m_size        = allocInfo.size;
    buffer.m_mapped_data = (u8*)allocInfo.pMappedData;

    return buffer;
  }

  auto vulkan_allocator::create_image(
    vk::ImageCreateInfo info,
    VmaMemoryUsage usage) -> vulkan_image
  {
    VkImageCreateInfo imageCreateInfo = info;

    VmaAllocationCreateInfo allocCreateInfo {};
    allocCreateInfo.usage = usage;
    allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    vulkan_image image;
    VmaAllocationInfo allocInfo {};

    vmaCreateImage(
      m_allocator,
      &imageCreateInfo,
      &allocCreateInfo,
      &image.m_image,
      &image.m_allocation,
      &allocInfo);

    vmaGetMemoryTypeProperties(
      m_allocator, allocInfo.memoryType, &image.m_flags);

    image.m_allocator   = m_allocator;
    image.m_size        = allocInfo.size;
    image.m_mapped_data = (u8*)allocInfo.pMappedData;

    return image;
  }

} // namespace yave::ui