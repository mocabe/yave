//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/vulkan/offscreen.hpp>
#include <yave/lib/vulkan/vulkan_util.hpp>
#include <yave/lib/vulkan/image.hpp>

namespace yave::vulkan {

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
      auto memReq = device.getImageMemoryRequirements(image.get());

      // device local memory
      vk::MemoryAllocateInfo info;
      info.allocationSize  = memReq.size;
      info.memoryTypeIndex = find_memory_type_index(
        memReq, vk::MemoryPropertyFlagBits::eDeviceLocal, physicalDevice);

      memory = device.allocateMemoryUnique(info);
      device.bindImageMemory(image.get(), memory.get(), 0);
    }

    auto range = image_subresource_range();

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

      image_layout_transfer_dst_to_color_attachment(cmd, image.get());
    }

    return {extent,
            format,
            format_texel_size(format) * extent.width * extent.height,
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

    image_layout_color_attachment_to_transfer_dst(cmd, dst.image.get());

    cmd.clearColorImage(
      dst.image.get(),
      vk::ImageLayout::eTransferDstOptimal,
      clearColor,
      image_subresource_range());

    image_layout_transfer_dst_to_color_attachment(cmd, dst.image.get());
  }

  void store_offscreen_frame_data(
    staging_buffer& staging,
    offscreen_frame_data& dst,
    const vk::Offset2D& offset,
    const vk::Extent2D& size,
    const std::byte* data,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice)
  {
    assert(staging.buffer.get());
    assert(dst.image.get());
    assert(offset.x + size.width <= dst.extent.width);
    assert(offset.y + size.height <= dst.extent.height);

    auto texel_size = format_texel_size(dst.format);
    auto buff_size  = size.width * size.height * texel_size;

    resize_staging_buffer(staging, buff_size, device, physicalDevice);

    auto buffer       = staging.buffer.get();
    auto bufferMemory = staging.memory.get();

    /* upload data */
    {
      void* ptr = device.mapMemory(bufferMemory, 0, buff_size);
      std::memcpy(ptr, data, buff_size);
      device.unmapMemory(bufferMemory);
    }

    // submit commands
    {
      auto stc = single_time_command(device, cmdQueue, cmdPool);
      auto cmd = stc.command_buffer();

      image_layout_color_attachment_to_transfer_dst(cmd, dst.image.get());

      vk::BufferImageCopy region;
      region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
      region.imageSubresource.layerCount = 1;
      region.imageOffset                 = vk::Offset3D {offset, 0};
      region.imageExtent                 = vk::Extent3D {size, 1};

      cmd.copyBufferToImage(
        buffer, dst.image.get(), vk::ImageLayout::eTransferDstOptimal, region);

      image_layout_transfer_dst_to_color_attachment(cmd, dst.image.get());
    }
  }

  void load_offscreen_frame_data(
    staging_buffer& staging,
    const offscreen_frame_data& src,
    const vk::Offset2D& offset,
    const vk::Extent2D& size,
    std::byte* dstData,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice)
  {
    assert(offset.x + size.width <= src.extent.width);
    assert(offset.y + size.height <= src.extent.height);

    auto texel_size = format_texel_size(src.format);
    auto buff_size  = size.width * size.height * texel_size;

    resize_staging_buffer(staging, buff_size, device, physicalDevice);

    auto buffer       = staging.buffer.get();
    auto bufferMemory = staging.memory.get();

    // image -> buffer
    {
      auto stc = single_time_command(device, cmdQueue, cmdPool);
      auto cmd = stc.command_buffer();

      image_layout_color_attachment_to_transfer_src(cmd, src.image.get());

      vk::BufferImageCopy region;
      region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
      region.imageSubresource.layerCount = 1;
      region.imageOffset                 = vk::Offset3D {offset, 0};
      region.imageExtent                 = vk::Extent3D {size, 1};

      cmd.copyImageToBuffer(
        src.image.get(), vk::ImageLayout::eTransferSrcOptimal, buffer, region);

      image_layout_transfer_src_to_color_attachment(cmd, src.image.get());
    }

    // buffer -> host memory
    {
      const void* ptr = device.mapMemory(bufferMemory, 0, buff_size);
      std::memcpy(dstData, ptr, buff_size);
      device.unmapMemory(bufferMemory);
    }
  }

  void store_offscreen_frame_data(
    const texture_data& src,
    const vk::Offset2D& srcOffset,
    offscreen_frame_data& dst,
    const vk::Offset2D& dstOffset,
    const vk::Extent2D& size,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice)
  {
    assert(srcOffset.x + size.width <= dst.extent.width);
    assert(srcOffset.y + size.height <= dst.extent.height);
    assert(dstOffset.x + size.width <= src.extent.width);
    assert(dstOffset.y + size.height <= src.extent.height);

    vk::ImageSubresourceLayers layer;
    layer.aspectMask = vk::ImageAspectFlagBits::eColor;
    layer.layerCount = 1;

    vk::ImageCopy region;
    region.srcSubresource = layer;
    region.srcOffset      = vk::Offset3D {srcOffset, 0};
    region.dstSubresource = layer;
    region.dstOffset      = vk::Offset3D {dstOffset, 0};
    region.extent         = vk::Extent3D {size, 1};

    {
      auto stc = single_time_command(device, cmdQueue, cmdPool);
      auto cmd = stc.command_buffer();

      image_layout_shader_read_to_transfer_src(cmd, src.image.get());
      image_layout_color_attachment_to_transfer_dst(cmd, dst.image.get());

      cmd.copyImage(
        src.image.get(),
        vk::ImageLayout::eTransferSrcOptimal,
        dst.image.get(),
        vk::ImageLayout::eTransferDstOptimal,
        region);

      image_layout_transfer_src_to_shader_read(cmd, src.image.get());
      image_layout_transfer_dst_to_color_attachment(cmd, dst.image.get());
    }
  }

  void load_offscreen_frame_data(
    const offscreen_frame_data& src,
    const vk::Offset2D& srcOffset,
    texture_data& dst,
    const vk::Offset2D& dstOffset,
    const vk::Extent2D& size,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice)
  {
    assert(srcOffset.x + size.width <= dst.extent.width);
    assert(srcOffset.y + size.height <= dst.extent.height);
    assert(dstOffset.x + size.width <= src.extent.width);
    assert(dstOffset.y + size.height <= src.extent.height);

    vk::ImageSubresourceLayers layer;
    layer.aspectMask = vk::ImageAspectFlagBits::eColor;
    layer.layerCount = 1;

    vk::ImageCopy region;
    region.srcSubresource = layer;
    region.srcOffset      = vk::Offset3D {srcOffset, 0};
    region.dstSubresource = layer;
    region.dstOffset      = vk::Offset3D {dstOffset, 0};
    region.extent         = vk::Extent3D {size, 1};

    {
      auto stc = single_time_command(device, cmdQueue, cmdPool);
      auto cmd = stc.command_buffer();

      image_layout_color_attachment_to_transfer_src(cmd, src.image.get());
      image_layout_shader_read_to_transfer_dst(cmd, dst.image.get());

      cmd.copyImage(
        src.image.get(),
        vk::ImageLayout::eTransferSrcOptimal,
        dst.image.get(),
        vk::ImageLayout::eTransferDstOptimal,
        region);

      image_layout_transfer_src_to_color_attachment(cmd, src.image.get());
      image_layout_transfer_dst_to_shader_read(cmd, dst.image.get());
    }
  }
} // namespace yave::vulkan