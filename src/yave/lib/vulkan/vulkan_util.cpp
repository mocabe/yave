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
    m_fence = m_device.createFenceUnique({});

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
  // upload_image

  /// Find memory type
  uint32_t find_memory_type(
    uint32_t typeBits,
    vk::MemoryPropertyFlags properties,
    const vk::PhysicalDevice& physicalDevice)
  {
    auto memProperties = physicalDevice.getMemoryProperties();
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
      if (
        typeBits & (1 << i) && (memProperties.memoryTypes[i].propertyFlags &
                                properties) == properties) {
        return i;
      }
    }
    throw std::runtime_error("Failed to find suitable memory type");
  }

  std::tuple<vk::UniqueImage, vk::UniqueImageView, vk::UniqueDeviceMemory>
    upload_image(
      const vk::Extent2D& extent,
      const vk::DeviceSize& byte_size,
      const uint8_t* data,
      const vk::CommandPool& commandPool,
      const vk::Queue queue,
      const vk::PhysicalDevice& physicalDevice,
      const vk::Device& device)
  {
    assert(data);

    vk::Extent3D imageExtent {extent, 1};

    /* create staging buffer */
    vk::UniqueBuffer buffer;
    {
      vk::BufferCreateInfo info;
      info.size        = byte_size;
      info.usage       = vk::BufferUsageFlagBits::eTransferSrc;
      info.sharingMode = vk::SharingMode::eExclusive;

      buffer = device.createBufferUnique(info);
    }

    /* create staging buffer memory */
    vk::UniqueDeviceMemory bufferMemory;
    {
      auto memReq = device.getBufferMemoryRequirements(buffer.get());
      vk::MemoryAllocateInfo info;
      info.allocationSize = memReq.size;
      // host visible, coherent
      info.memoryTypeIndex = find_memory_type(
        memReq.memoryTypeBits,
        vk::MemoryPropertyFlagBits::eHostVisible |
          vk::MemoryPropertyFlagBits::eHostCoherent,
        physicalDevice);

      bufferMemory = device.allocateMemoryUnique(info);
      device.bindBufferMemory(buffer.get(), bufferMemory.get(), 0);
    }

    /* create image */
    vk::UniqueImage image;
    {
      vk::ImageCreateInfo info;
      info.imageType   = vk::ImageType::e2D;
      info.format      = vk::Format::eR8G8B8A8Unorm;
      info.extent      = imageExtent;
      info.mipLevels   = 1;
      info.arrayLayers = 1;
      info.samples     = vk::SampleCountFlagBits::e1;
      info.tiling      = vk::ImageTiling::eOptimal;
      info.usage =
        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
      info.sharingMode   = vk::SharingMode::eExclusive;
      info.initialLayout = vk::ImageLayout::eUndefined;

      image = device.createImageUnique(info);
    }

    /* map image to memory */
    vk::UniqueDeviceMemory imageMemory;
    {
      auto memReq = device.getImageMemoryRequirements(image.get());
      vk::MemoryAllocateInfo info;
      info.allocationSize = memReq.size;
      // device local
      info.memoryTypeIndex = find_memory_type(
        memReq.memoryTypeBits,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        physicalDevice);
      imageMemory = device.allocateMemoryUnique(info);
      device.bindImageMemory(image.get(), imageMemory.get(), 0);
    }

    /* create subresource range */
    vk::ImageSubresourceRange subresourceRange;
    {
      subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      subresourceRange.levelCount = 1;
      subresourceRange.layerCount = 1;
    }

    /* upload data */
    {
      void* ptr = device.mapMemory(bufferMemory.get(), 0, byte_size);
      std::memcpy(ptr, data, byte_size);
      device.unmapMemory(bufferMemory.get());
    }

    auto singleTimeCommand = single_time_command(device, queue, commandPool);
    auto cmd               = singleTimeCommand.command_buffer();

    /* image layout: undefined -> transfer dst optimal */
    {
      vk::ImageMemoryBarrier barrier;
      barrier.oldLayout           = vk::ImageLayout::eUndefined;
      barrier.newLayout           = vk::ImageLayout::eTransferDstOptimal;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.image               = image.get();
      barrier.subresourceRange    = subresourceRange;
      barrier.dstAccessMask       = vk::AccessFlagBits::eTransferWrite;

      vk::PipelineStageFlags srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
      vk::PipelineStageFlags dstStage = vk::PipelineStageFlagBits::eTransfer;

      cmd.pipelineBarrier(
        srcStage, dstStage, vk::DependencyFlags(), {}, {}, barrier);
    }

    /* copy: buffer -> image */
    {
      vk::BufferImageCopy region;
      region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
      region.imageSubresource.layerCount = 1;
      region.imageExtent                 = imageExtent;

      cmd.copyBufferToImage(
        buffer.get(),
        image.get(),
        vk::ImageLayout::eTransferDstOptimal,
        region);
    }

    /* image layout: transfer dst optimal -> shader read only */
    {
      vk::ImageMemoryBarrier barrier;
      barrier.oldLayout        = vk::ImageLayout::eTransferDstOptimal;
      barrier.newLayout        = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.image            = image.get();
      barrier.subresourceRange = subresourceRange;
      barrier.srcAccessMask    = vk::AccessFlagBits::eTransferWrite;
      barrier.dstAccessMask    = vk::AccessFlagBits::eShaderRead;

      vk::PipelineStageFlags srcStage = vk::PipelineStageFlagBits::eTransfer;
      vk::PipelineStageFlags dstStage =
        vk::PipelineStageFlagBits::eFragmentShader;

      cmd.pipelineBarrier(
        srcStage, dstStage, vk::DependencyFlags(), {}, {}, barrier);
    }

    /* create image view */
    vk::UniqueImageView imageView;
    {
      vk::ImageViewCreateInfo info;
      info.image            = image.get();
      info.viewType         = vk::ImageViewType::e2D;
      info.format           = vk::Format::eR8G8B8A8Unorm;
      info.subresourceRange = subresourceRange;
      imageView             = device.createImageViewUnique(info);
    }

    return {std::move(image), std::move(imageView), std::move(imageMemory)};
  }

  // -----------------------------------------
  // create_descriptor

  vk::UniqueDescriptorSet create_image_descriptor(
    const vk::ImageView& image,
    const vk::DescriptorSetLayout& layout,
    const vk::DescriptorPool& pool,
    const vk::Device& device)
  {
    /* create new descriptor */
    vk::UniqueDescriptorSet set;
    {
      vk::DescriptorSetAllocateInfo info;
      info.descriptorPool     = pool;
      info.descriptorSetCount = 1;
      info.pSetLayouts        = &layout;

      auto sets = device.allocateDescriptorSetsUnique(info);
      assert(sets.size() == 1);
      set = std::move(sets.front());
    }

    /* update descriptor with image view */
    {
      vk::DescriptorImageInfo info;
      info.imageView   = image;
      info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

      vk::WriteDescriptorSet write;
      write.descriptorCount = 1;
      write.dstSet          = set.get();
      write.descriptorType  = vk::DescriptorType::eCombinedImageSampler;
      write.pImageInfo      = &info;
      device.updateDescriptorSets(write, {});
    }

    return set;
  }

} // namespace yave