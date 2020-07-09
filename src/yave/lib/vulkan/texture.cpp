//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/vulkan/texture.hpp>
#include <yave/lib/vulkan/vulkan_util.hpp>

namespace yave::vulkan {

  namespace {

    using namespace yave::vulkan;

    auto textureImageSubresourceRange()
    {
      vk::ImageSubresourceRange range;
      range.aspectMask = vk::ImageAspectFlagBits::eColor;
      range.layerCount = 1;
      range.levelCount = 1;
      return range;
    }

    void textureLayoutTransition(
      vk::CommandBuffer cmd,
      vk::Image image,
      vk::ImageLayout oldLayout,
      vk::ImageLayout newLayout,
      vk::AccessFlags srcAccessMask,
      vk::AccessFlags dstAccessMask,
      vk::PipelineStageFlags srcStage,
      vk::PipelineStageFlags dstStage)
    {
      auto range = textureImageSubresourceRange();

      vk::ImageMemoryBarrier barrier;
      barrier.image            = image;
      barrier.subresourceRange = range;
      barrier.oldLayout        = oldLayout;
      barrier.newLayout        = newLayout;
      barrier.srcAccessMask    = srcAccessMask;
      barrier.dstAccessMask    = dstAccessMask;

      cmd.pipelineBarrier(srcStage, dstStage, {}, {}, {}, barrier);
    }

    void textureLayoutShaderReadOnlyToTransferDst(
      const vk::CommandBuffer& cmd,
      const vk::Image& image)
    {
      textureLayoutTransition(
        cmd,
        image,
        vk::ImageLayout::eShaderReadOnlyOptimal,
        vk::ImageLayout::eTransferDstOptimal,
        vk::AccessFlagBits::eShaderRead,
        vk::AccessFlagBits::eTransferWrite,
        vk::PipelineStageFlagBits::eFragmentShader,
        vk::PipelineStageFlagBits::eTransfer);
    }

    void textureLayoutShaderReadOnlyToTransferSrc(
      const vk::CommandBuffer& cmd,
      const vk::Image& image)
    {
      textureLayoutTransition(
        cmd,
        image,
        vk::ImageLayout::eShaderReadOnlyOptimal,
        vk::ImageLayout::eTransferSrcOptimal,
        vk::AccessFlagBits::eShaderRead,
        vk::AccessFlagBits::eTransferRead,
        vk::PipelineStageFlagBits::eFragmentShader,
        vk::PipelineStageFlagBits::eTransfer);
    }

    void textureLayoutTransferDstToShaderReadOnly(
      const vk::CommandBuffer& cmd,
      const vk::Image& image)
    {
      textureLayoutTransition(
        cmd,
        image,
        vk::ImageLayout::eTransferDstOptimal,
        vk::ImageLayout::eShaderReadOnlyOptimal,
        vk::AccessFlagBits::eTransferWrite,
        vk::AccessFlagBits::eShaderRead,
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eFragmentShader);
    }

    void textureLayoutTransferSrcToShaderReadOnly(
      const vk::CommandBuffer& cmd,
      const vk::Image& image)
    {
      textureLayoutTransition(
        cmd,
        image,
        vk::ImageLayout::eTransferSrcOptimal,
        vk::ImageLayout::eShaderReadOnlyOptimal,
        vk::AccessFlagBits::eTransferRead,
        vk::AccessFlagBits::eShaderRead,
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eFragmentShader);
    }

    void textureLayoutUndefinedToTransferDst(
      const vk::CommandBuffer& cmd,
      const vk::Image& image)
    {
      textureLayoutTransition(
        cmd,
        image,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal,
        vk::AccessFlagBits(),
        vk::AccessFlagBits::eTransferWrite,
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eTransfer);
    }

    [[maybe_unused]]
    void textureLayoutUndefinedToShaderReadOnly(
      const vk::CommandBuffer& cmd,
      const vk::Image& image)
    {
      textureLayoutTransition(
        cmd,
        image,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eShaderReadOnlyOptimal,
        vk::AccessFlagBits(),
        vk::AccessFlagBits::eShaderRead,
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eFragmentShader);
    }
  } // namespace

  auto create_texture_data(
    const uint32_t& width,
    const uint32_t& height,
    const vk::Format& format,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice) -> texture_data
  {
    vk::UniqueImage image;
    {
      vk::ImageCreateInfo info;
      info.imageType   = vk::ImageType::e2D;
      info.format      = format;
      info.extent      = vk::Extent3D {width, height, 1};
      info.mipLevels   = 1;
      info.arrayLayers = 1;
      info.samples     = vk::SampleCountFlagBits::e1;
      info.tiling      = vk::ImageTiling::eOptimal;
      info.usage       = vk::ImageUsageFlagBits::eSampled
                   | vk::ImageUsageFlagBits::eTransferDst
                   | vk::ImageUsageFlagBits::eTransferSrc;
      info.sharingMode   = vk::SharingMode::eExclusive;
      info.initialLayout = vk::ImageLayout::eUndefined;

      image = device.createImageUnique(info);
    }

    vk::UniqueDeviceMemory memory;
    {
      auto memReq = device.getImageMemoryRequirements(image.get());
      vk::MemoryAllocateInfo info;
      info.allocationSize  = memReq.size;
      info.memoryTypeIndex = find_memory_type_index(
        memReq, vk::MemoryPropertyFlagBits::eDeviceLocal, physicalDevice);
      memory = device.allocateMemoryUnique(info);
      device.bindImageMemory(image.get(), memory.get(), 0);
    }

    auto range = textureImageSubresourceRange();

    vk::UniqueImageView view;
    {
      vk::ImageViewCreateInfo info;
      info.image            = image.get();
      info.viewType         = vk::ImageViewType::e2D;
      info.format           = format;
      info.subresourceRange = range;
      view                  = device.createImageViewUnique(info);
    }

    {
      auto stc = single_time_command(device, cmdQueue, cmdPool);
      auto cmd = stc.command_buffer();

      // set (0,1,0,1) for debug
      if constexpr (is_debug) {
        textureLayoutUndefinedToTransferDst(cmd, image.get());
        if constexpr (is_debug)
          cmd.clearColorImage(
            image.get(),
            vk::ImageLayout::eTransferDstOptimal,
            vk::ClearColorValue(std::array {0.f, 1.f, 0.f, 1.f}),
            range);
        textureLayoutTransferDstToShaderReadOnly(cmd, image.get());
      } else
        textureLayoutUndefinedToShaderReadOnly(cmd, image.get());
    }

    return {
      width,
      height,
      format,
      format_texel_size(format) * width * height,
      std::move(image),
      std::move(view),
      std::move(memory)};
  }

  auto clone_texture_data(
    const texture_data& tex,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice) -> texture_data
  {
    vk::UniqueImage image;
    {
      vk::ImageCreateInfo info;
      info.imageType   = vk::ImageType::e2D;
      info.format      = tex.format;
      info.extent      = vk::Extent3D {tex.width, tex.height, 1};
      info.mipLevels   = 1;
      info.arrayLayers = 1;
      info.samples     = vk::SampleCountFlagBits::e1;
      info.tiling      = vk::ImageTiling::eOptimal;
      info.usage       = vk::ImageUsageFlagBits::eSampled
                   | vk::ImageUsageFlagBits::eTransferDst
                   | vk::ImageUsageFlagBits::eTransferSrc;
      info.sharingMode   = vk::SharingMode::eExclusive;
      info.initialLayout = vk::ImageLayout::eUndefined;

      image = device.createImageUnique(info);
    }

    vk::UniqueDeviceMemory memory;
    {
      auto memReq = device.getImageMemoryRequirements(image.get());
      vk::MemoryAllocateInfo info;
      info.allocationSize  = memReq.size;
      info.memoryTypeIndex = vulkan::find_memory_type_index(
        memReq, vk::MemoryPropertyFlagBits::eDeviceLocal, physicalDevice);
      memory = device.allocateMemoryUnique(info);
      device.bindImageMemory(image.get(), memory.get(), 0);
    }

    vk::ImageSubresourceRange range;
    {
      range.aspectMask = vk::ImageAspectFlagBits::eColor;
      range.layerCount = 1;
      range.levelCount = 1;
    }

    vk::ImageSubresourceLayers layer;
    {
      layer.aspectMask = vk::ImageAspectFlagBits::eColor;
      layer.layerCount = 1;
    }

    vk::UniqueImageView view;
    {
      vk::ImageViewCreateInfo info;
      info.image            = image.get();
      info.viewType         = vk::ImageViewType::e2D;
      info.format           = tex.format;
      info.subresourceRange = range;
      view                  = device.createImageViewUnique(info);
    }

    {
      auto stc = vulkan::single_time_command(device, cmdQueue, cmdPool);
      auto cmd = stc.command_buffer();

      textureLayoutShaderReadOnlyToTransferSrc(cmd, tex.image.get());
      textureLayoutUndefinedToTransferDst(cmd, image.get());

      // copy data
      {
        vk::ImageCopy region;
        region.srcSubresource = layer;
        region.dstSubresource = layer;
        region.extent         = vk::Extent3D {tex.width, tex.height, 1};

        cmd.copyImage(
          tex.image.get(),
          vk::ImageLayout::eTransferSrcOptimal,
          image.get(),
          vk::ImageLayout::eTransferDstOptimal,
          region);
      }

      textureLayoutTransferSrcToShaderReadOnly(cmd, tex.image.get());
      textureLayoutTransferDstToShaderReadOnly(cmd, image.get());
    }

    return {
      tex.width,
      tex.height,
      tex.format,
      tex.size,
      std::move(image),
      std::move(view),
      std::move(memory)};
  }

  void clear_texture_data(
    texture_data& dst,
    const vk::ClearColorValue& clearColor,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::Device& device,
    const vk::PhysicalDevice&)
  {
    auto stc = single_time_command(device, cmdQueue, cmdPool);
    auto cmd = stc.command_buffer();

    textureLayoutShaderReadOnlyToTransferDst(cmd, dst.image.get());

    cmd.clearColorImage(
      dst.image.get(),
      vk::ImageLayout::eTransferDstOptimal,
      clearColor,
      textureImageSubresourceRange());

    textureLayoutTransferDstToShaderReadOnly(cmd, dst.image.get());
  }

  void store_texture_data(
    staging_buffer& staging,
    texture_data& dst,
    const vk::Offset2D& offset,
    const vk::Extent2D& size,
    const uint8_t* data,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice)
  {
    assert(staging.buffer.get());
    assert(dst.image.get());
    assert(offset.x + size.width <= dst.width);
    assert(offset.y + size.height <= dst.height);

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

      textureLayoutShaderReadOnlyToTransferDst(cmd, dst.image.get());

      vk::BufferImageCopy region;
      region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
      region.imageSubresource.layerCount = 1;
      // area to write
      region.imageOffset = vk::Offset3D {offset.x, offset.y, 0};
      region.imageExtent = vk::Extent3D {size.width, size.height, 1};

      cmd.copyBufferToImage(
        buffer, dst.image.get(), vk::ImageLayout::eTransferDstOptimal, region);

      textureLayoutTransferDstToShaderReadOnly(cmd, dst.image.get());
    }
  }

  void read_texture_data(
    staging_buffer& staging,
    const texture_data& src,
    const vk::Offset2D& offset,
    const vk::Extent2D& size,
    uint8_t* data,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice)
  {
    assert(offset.x + size.width <= src.width);
    assert(offset.y + size.height <= src.height);

    auto texel_size = format_texel_size(src.format);
    auto buff_size  = size.width * size.height * texel_size;

    resize_staging_buffer(staging, buff_size, device, physicalDevice);

    auto buffer       = staging.buffer.get();
    auto bufferMemory = staging.memory.get();

    // submit commands
    {
      auto stc = single_time_command(device, cmdQueue, cmdPool);
      auto cmd = stc.command_buffer();

      textureLayoutShaderReadOnlyToTransferSrc(cmd, src.image.get());

      vk::BufferImageCopy region;
      region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
      region.imageSubresource.layerCount = 1;
      // area to write
      region.imageOffset = vk::Offset3D {offset.x, offset.y, 0};
      region.imageExtent = vk::Extent3D {size.width, size.height, 1};

      cmd.copyImageToBuffer(
        src.image.get(), vk::ImageLayout::eTransferSrcOptimal, buffer, region);

      textureLayoutTransferSrcToShaderReadOnly(cmd, src.image.get());
    }

    /* upload data */
    {
      const void* ptr = device.mapMemory(bufferMemory, 0, buff_size);
      std::memcpy(data, ptr, buff_size);
      device.unmapMemory(bufferMemory);
    }
  }

} // namespace yave::vulkan