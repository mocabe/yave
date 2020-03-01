//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/vulkan/offscreen.hpp>
#include <yave/lib/vulkan/vulkan_util.hpp>

namespace yave::vulkan {

  namespace {

    auto offscreenBufferImageResourceRange()
    {
      vk::ImageSubresourceRange subResourceRange;
      subResourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
      subResourceRange.levelCount = 1;
      subResourceRange.layerCount = 1;
      return subResourceRange;
    }

    auto offscreenBufferLayoutColorAttachmentToTransferDst(
      const vk::CommandBuffer& cmd,
      const vk::Image& image)
    {
      auto range = offscreenBufferImageResourceRange();

      vk::ImageMemoryBarrier barrier;
      barrier.image            = image;
      barrier.subresourceRange = range;
      barrier.oldLayout        = vk::ImageLayout::eColorAttachmentOptimal;
      barrier.newLayout        = vk::ImageLayout::eTransferDstOptimal;
      barrier.srcAccessMask    = vk::AccessFlagBits::eColorAttachmentRead
                              | vk::AccessFlagBits::eColorAttachmentWrite;
      barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

      vk::PipelineStageFlags srcStage =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;
      vk::PipelineStageFlags dstStage = vk::PipelineStageFlagBits::eTransfer;

      cmd.pipelineBarrier(srcStage, dstStage, {}, {}, {}, barrier);
    }

    auto offscreenBufferLayoutColorAttachmentToTransferSrc(
      const vk::CommandBuffer& cmd,
      const vk::Image& image)
    {
      auto range = offscreenBufferImageResourceRange();

      vk::ImageMemoryBarrier barrier;
      barrier.image            = image;
      barrier.subresourceRange = range;
      barrier.oldLayout        = vk::ImageLayout::eColorAttachmentOptimal;
      barrier.newLayout        = vk::ImageLayout::eTransferSrcOptimal;
      barrier.srcAccessMask    = vk::AccessFlagBits::eColorAttachmentRead
                              | vk::AccessFlagBits::eColorAttachmentWrite;
      barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

      vk::PipelineStageFlags srcStage =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;
      vk::PipelineStageFlags dstStage = vk::PipelineStageFlagBits::eTransfer;

      cmd.pipelineBarrier(srcStage, dstStage, {}, {}, {}, barrier);
    }

    auto offscreenBufferLayoutTransferDstToColorAttachment(
      const vk::CommandBuffer& cmd,
      const vk::Image& image)
    {
      auto range = offscreenBufferImageResourceRange();

      vk::ImageMemoryBarrier barrier;
      barrier.image            = image;
      barrier.subresourceRange = range;
      barrier.oldLayout        = vk::ImageLayout::eTransferDstOptimal;
      barrier.newLayout        = vk::ImageLayout::eColorAttachmentOptimal;
      barrier.srcAccessMask    = vk::AccessFlagBits::eTransferWrite;
      barrier.dstAccessMask    = vk::AccessFlagBits::eColorAttachmentRead
                              | vk::AccessFlagBits::eColorAttachmentWrite;

      vk::PipelineStageFlags srcStage = vk::PipelineStageFlagBits::eTransfer;
      vk::PipelineStageFlags dstStage =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;

      cmd.pipelineBarrier(srcStage, dstStage, {}, {}, {}, barrier);
    }

    auto offscreenBufferLayoutTransferSrcToColorAttachment(
      const vk::CommandBuffer& cmd,
      const vk::Image& image)
    {
      auto range = offscreenBufferImageResourceRange();

      vk::ImageMemoryBarrier barrier;
      barrier.image            = image;
      barrier.subresourceRange = range;
      barrier.oldLayout        = vk::ImageLayout::eTransferSrcOptimal;
      barrier.newLayout        = vk::ImageLayout::eColorAttachmentOptimal;
      barrier.srcAccessMask    = vk::AccessFlagBits::eTransferRead;
      barrier.dstAccessMask    = vk::AccessFlagBits::eColorAttachmentRead
                              | vk::AccessFlagBits::eColorAttachmentWrite;

      vk::PipelineStageFlags srcStage = vk::PipelineStageFlagBits::eTransfer;
      vk::PipelineStageFlags dstStage =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;

      cmd.pipelineBarrier(srcStage, dstStage, {}, {}, {}, barrier);
    }
  } // namespace

  auto create_offscreen_frame_data(
    const vk::Extent2D& extent,
    const vk::Format& format,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::RenderPass& renderPass,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice) -> offscreen_frame_data
  {
    vk::UniqueImage image;
    {
      vk::ImageCreateInfo info;
      info.imageType   = vk::ImageType::e2D;
      info.format      = format;
      info.extent      = vk::Extent3D {extent, 1};
      info.mipLevels   = 1;
      info.arrayLayers = 1;
      info.samples     = vk::SampleCountFlagBits::e1;
      info.tiling      = vk::ImageTiling::eOptimal;
      info.usage = vk::ImageUsageFlagBits::eColorAttachment // as frame buffer
                   | vk::ImageUsageFlagBits::eTransferDst   // as copy dst
                   | vk::ImageUsageFlagBits::eTransferSrc;  // as copy src
      info.sharingMode   = vk::SharingMode::eExclusive;
      info.initialLayout = vk::ImageLayout::eUndefined;

      image = device.createImageUnique(info);
    }

    vk::UniqueDeviceMemory memory;
    {
      auto req = device.getImageMemoryRequirements(image.get());

      // device local memory
      vk::MemoryAllocateInfo info;
      info.allocationSize  = req.size;
      info.memoryTypeIndex = find_memory_type_index(
        req, vk::MemoryPropertyFlagBits::eDeviceLocal, physicalDevice);

      memory = device.allocateMemoryUnique(info);
      device.bindImageMemory(image.get(), memory.get(), 0);
    }

    auto range = offscreenBufferImageResourceRange();

    vk::UniqueImageView view;
    {
      vk::ImageViewCreateInfo info;
      info.image            = image.get();
      info.viewType         = vk::ImageViewType::e2D;
      info.format           = format;
      info.subresourceRange = range;

      view = device.createImageViewUnique(info);
    }

    vk::UniqueFramebuffer buffer;
    {
      vk::FramebufferCreateInfo info;
      info.renderPass      = renderPass;
      info.attachmentCount = 1;
      info.pAttachments    = &view.get();
      info.width           = extent.width;
      info.height          = extent.height;
      info.layers          = 1;

      buffer = device.createFramebufferUnique(info);
    }

    {
      auto stc = single_time_command(device, cmdQueue, cmdPool);
      auto cmd = stc.command_buffer();

      // Undefined -> TransferDstOptinal
      {
        vk::ImageMemoryBarrier barrier;
        barrier.oldLayout        = vk::ImageLayout::eUndefined;
        barrier.newLayout        = vk::ImageLayout::eTransferDstOptimal;
        barrier.image            = image.get();
        barrier.subresourceRange = range;
        barrier.dstAccessMask    = vk::AccessFlagBits::eTransferWrite;

        vk::PipelineStageFlags srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
        vk::PipelineStageFlags dstStage = vk::PipelineStageFlagBits::eTransfer;

        cmd.pipelineBarrier(srcStage, dstStage, {}, {}, {}, barrier);
      }

      // set (0,1,0,1) for debug
      cmd.clearColorImage(
        image.get(),
        vk::ImageLayout::eTransferDstOptimal,
        vk::ClearColorValue(std::array {0.f, 1.f, 0.f, 1.f}),
        range);

      offscreenBufferLayoutTransferDstToColorAttachment(cmd, image.get());
    }

    return {extent,
            format,
            std::move(image),
            std::move(memory),
            std::move(view),
            std::move(buffer)};
  }

  void clear_offscreen_frame_data(
    offscreen_frame_data& dst,
    const vk::ClearColorValue& clearColor,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::Device& device,
    const vk::PhysicalDevice&)
  {
    auto stc = single_time_command(device, cmdQueue, cmdPool);
    auto cmd = stc.command_buffer();

    offscreenBufferLayoutColorAttachmentToTransferDst(cmd, dst.image.get());

    cmd.clearColorImage(
      dst.image.get(),
      vk::ImageLayout::eTransferDstOptimal,
      clearColor,
      offscreenBufferImageResourceRange());

    offscreenBufferLayoutTransferDstToColorAttachment(cmd, dst.image.get());
  }

  void store_offscreen_frame_data(
    offscreen_frame_data& dst,
    const std::byte* srcData,
    const vk::DeviceSize& srcSize,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice)
  {
    // create staging buffer
    vk::UniqueBuffer buffer;
    {
      vk::BufferCreateInfo info;
      info.size        = srcSize;
      info.usage       = vk::BufferUsageFlagBits::eTransferSrc;
      info.sharingMode = vk::SharingMode::eExclusive;

      buffer = device.createBufferUnique(info);
    }

    // create memory for stating buffer
    vk::UniqueDeviceMemory bufferMemory;
    {
      auto memReq = device.getBufferMemoryRequirements(buffer.get());
      vk::MemoryAllocateInfo info;
      info.allocationSize = memReq.size;
      // should be host visible, coherent
      info.memoryTypeIndex = find_memory_type_index(
        memReq,
        vk::MemoryPropertyFlagBits::eHostVisible
          | vk::MemoryPropertyFlagBits::eHostCoherent,
        physicalDevice);

      bufferMemory = device.allocateMemoryUnique(info);
      device.bindBufferMemory(buffer.get(), bufferMemory.get(), 0);
    }

    /* upload data */
    {
      void* ptr = device.mapMemory(bufferMemory.get(), 0, srcSize);
      std::memcpy(ptr, srcData, srcSize);
      device.unmapMemory(bufferMemory.get());
    }

    // submit commands
    {
      auto stc = single_time_command(device, cmdQueue, cmdPool);
      auto cmd = stc.command_buffer();

      offscreenBufferLayoutColorAttachmentToTransferDst(cmd, dst.image.get());

      {
        vk::BufferImageCopy region;
        region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.layerCount = 1;
        region.imageExtent                 = vk::Extent3D {dst.extent, 1};

        cmd.copyBufferToImage(
          buffer.get(),
          dst.image.get(),
          vk::ImageLayout::eTransferDstOptimal,
          region);
      }

      offscreenBufferLayoutTransferDstToColorAttachment(cmd, dst.image.get());
    }
  }

  void load_offscreen_frame_data(
    const offscreen_frame_data& src,
    std::byte* dstData,
    const vk::DeviceSize& dstSize,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice)
  {
    // staging buffer
    vk::UniqueBuffer buffer;
    {
      vk::BufferCreateInfo info;
      info.size        = dstSize;
      info.usage       = vk::BufferUsageFlagBits::eTransferDst;
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
          | vk::MemoryPropertyFlagBits::eHostCoherent,
        physicalDevice);

      bufferMemory = device.allocateMemoryUnique(info);
      device.bindBufferMemory(buffer.get(), bufferMemory.get(), 0);
    }

    // image -> buffer
    {
      auto stc = single_time_command(device, cmdQueue, cmdPool);
      auto cmd = stc.command_buffer();

      offscreenBufferLayoutColorAttachmentToTransferSrc(cmd, src.image.get());

      {
        vk::BufferImageCopy region;
        region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.layerCount = 1;
        region.imageExtent                 = vk::Extent3D {src.extent, 1};

        cmd.copyImageToBuffer(
          src.image.get(),
          vk::ImageLayout::eTransferSrcOptimal,
          buffer.get(),
          region);
      }

      offscreenBufferLayoutTransferSrcToColorAttachment(cmd, src.image.get());
    }

    // buffer -> host memory
    {
      const void* ptr = device.mapMemory(bufferMemory.get(), 0, dstSize);
      std::memcpy(dstData, ptr, dstSize);
      device.unmapMemory(bufferMemory.get());
    }
  }
} // namespace yave::vulkan