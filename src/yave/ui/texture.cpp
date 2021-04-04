//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/ui/texture.hpp>

namespace {

  auto imageSubresourceRange()
  {
    return vk::ImageSubresourceRange()
      .setAspectMask(vk::ImageAspectFlagBits::eColor)
      .setLayerCount(1)
      .setLevelCount(1);
  }
}

namespace yave::ui {

  void texture::layout_transition(
    vk::CommandBuffer cmd,
    vk::ImageLayout oldLayout,
    vk::ImageLayout newLayout,
    vk::AccessFlags srcAccessMask,
    vk::AccessFlags dstAccessMask,
    vk::PipelineStageFlags srcStage,
    vk::PipelineStageFlags dstStage)
  {
    auto& device = m_allocator.device();
    auto range   = imageSubresourceRange();

    auto barrier = vk::ImageMemoryBarrier()
                     .setImage(m_image.image())
                     .setSubresourceRange(range)
                     .setOldLayout(oldLayout)
                     .setNewLayout(newLayout)
                     .setSrcQueueFamilyIndex(device.graphics_queue_family())
                     .setDstQueueFamilyIndex(device.graphics_queue_family())
                     .setSrcAccessMask(srcAccessMask)
                     .setDstAccessMask(dstAccessMask);

    cmd.pipelineBarrier(srcStage, dstStage, {}, {}, {}, barrier);
  }

  texture::texture(
    u32 width,
    u32 height,
    vk::Format format,
    vulkan_allocator& allocator)
    : m_allocator {allocator}
  {
    auto& device = allocator.device();

    {
      auto info = vk::ImageCreateInfo()
                    .setExtent({width, height, 1})
                    .setImageType(vk::ImageType::e2D)
                    .setFormat(format)
                    .setMipLevels(1)
                    .setArrayLayers(1)
                    .setSamples(vk::SampleCountFlagBits::e1)
                    .setTiling(vk::ImageTiling::eOptimal)
                    .setUsage(
                      vk::ImageUsageFlagBits::eSampled |     //
                      vk::ImageUsageFlagBits::eTransferDst | //
                      vk::ImageUsageFlagBits::eTransferSrc)
                    .setSharingMode(vk::SharingMode::eExclusive)
                    .setInitialLayout(vk::ImageLayout::eUndefined);

      m_image       = allocator.create_image(info, VMA_MEMORY_USAGE_GPU_ONLY);
      m_create_info = info;
      m_current_queue_family = 0; // XXX: Is this okay to be zero?
      m_current_layout       = info.initialLayout;
    }

    {
      auto range = imageSubresourceRange();
      auto info  = vk::ImageViewCreateInfo()
                    .setImage(m_image.image())
                    .setViewType(vk::ImageViewType::e2D)
                    .setFormat(format)
                    .setSubresourceRange(range);

      m_image_view = device.device().createImageViewUnique(info);
    }

    {
      auto stc = vulkan::single_time_command(
        device.device(),
        device.graphics_queue(),
        device.graphics_command_pool());

      auto cmd = stc.command_buffer();

      // layout: Undefined -> ShaderReadOnlyOptimal
      layout_transition(
        cmd,
        m_current_layout,
        vk::ImageLayout::eShaderReadOnlyOptimal,
        vk::AccessFlags(),
        vk::AccessFlagBits::eShaderRead,
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eFragmentShader);
    }
  }

  void texture::clear_color(const color& color)
  {
    auto& device = m_allocator.device();

    auto stc = vulkan::single_time_command(
      device.device(), device.graphics_queue(), device.graphics_command_pool());

    auto cmd   = stc.command_buffer();
    auto range = imageSubresourceRange();

    // layout: ShaderRead -> Transfer
    layout_transition(
      cmd,
      m_current_layout,
      vk::ImageLayout::eTransferDstOptimal,
      vk::AccessFlagBits::eShaderRead,
      vk::AccessFlagBits::eTransferWrite,
      vk::PipelineStageFlagBits::eFragmentShader,
      vk::PipelineStageFlagBits::eTransfer);

    // requires graphics support for queue family
    cmd.clearColorImage(
      m_image.image(),
      vk::ImageLayout::eTransferDstOptimal,
      vk::ClearColorValue().setFloat32({color.r, color.g, color.b, color.a}),
      range);

    // layout: Transfer -> ShaderRead
    layout_transition(
      cmd,
      m_current_layout,
      vk::ImageLayout::eShaderReadOnlyOptimal,
      vk::AccessFlagBits::eTransferWrite,
      vk::AccessFlagBits::eShaderRead,
      vk::PipelineStageFlagBits::eTransfer,
      vk::PipelineStageFlagBits::eFragmentShader);
  }

  void texture::store(
    staging_buffer& staging,
    const vk::Offset2D& offset,
    const vk::Extent2D& extent,
    std::span<const u8> src)
  {
    auto texel_size = vulkan::format_texel_size(format());
    auto buff_size  = extent.width * extent.height * texel_size;
    staging.resize(buff_size);

    // store data to staging buffer
    staging.store(0, std::min(buff_size, src.size()), src.data());

    auto& device = m_allocator.device();

    // upload data to GPU
    {
      auto stc = vulkan::single_time_command(
        device.device(),
        device.graphics_queue(),
        device.graphics_command_pool());

      auto cmd = stc.command_buffer();

      // layout: ShaderRead -> TransferWrite
      layout_transition(
        cmd,
        m_current_layout,
        vk::ImageLayout::eTransferDstOptimal,
        vk::AccessFlagBits::eShaderRead,
        vk::AccessFlagBits::eTransferWrite,
        vk::PipelineStageFlagBits::eFragmentShader,
        vk::PipelineStageFlagBits::eTransfer);

      // copy buffer
      {
        auto region = vk::BufferImageCopy()
                        .setImageOffset({offset.x, offset.y, 0})
                        .setImageExtent({extent.width, extent.height, 1})
                        .setImageSubresource(
                          vk::ImageSubresourceLayers()
                            .setAspectMask(vk::ImageAspectFlagBits::eColor)
                            .setLayerCount(1));

        cmd.copyBufferToImage(
          staging.buffer(),
          m_image.image(),
          vk::ImageLayout::eTransferDstOptimal,
          region);
      }

      // layout: TransferWrite -> ShaderRead
      layout_transition(
        cmd,
        m_current_layout,
        vk::ImageLayout::eShaderReadOnlyOptimal,
        vk::AccessFlagBits::eTransferWrite,
        vk::AccessFlagBits::eShaderRead,
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eFragmentShader);
    }
  }

  void texture::load(
    staging_buffer& staging,
    const vk::Offset2D& offset,
    const vk::Extent2D& extent,
    std::span<u8> dst)
  {
    auto texel_size = vulkan::format_texel_size(format());
    auto buff_size  = extent.width * extent.height * texel_size;
    staging.resize(buff_size);

    auto& device = m_allocator.device();

    // load data from GPU
    {
      auto stc = vulkan::single_time_command(
        device.device(),
        device.graphics_queue(),
        device.graphics_command_pool());

      auto cmd = stc.command_buffer();

      // layout: ShaderRead -> TransferRead
      layout_transition(
        cmd,
        m_current_layout,
        vk::ImageLayout::eTransferSrcOptimal,
        vk::AccessFlagBits::eShaderRead,
        vk::AccessFlagBits::eTransferRead,
        vk::PipelineStageFlagBits::eFragmentShader,
        vk::PipelineStageFlagBits::eTransfer);

      // copy image
      {
        auto region = vk::BufferImageCopy()
                        .setImageOffset({offset.x, offset.y, 0})
                        .setImageExtent({extent.width, extent.height, 1})
                        .setImageSubresource(
                          vk::ImageSubresourceLayers()
                            .setAspectMask(vk::ImageAspectFlagBits::eColor)
                            .setLayerCount(1));

        cmd.copyImageToBuffer(
          m_image.image(),
          vk::ImageLayout::eTransferSrcOptimal,
          staging.buffer(),
          region);
      }

      // layout: TransferRead -> ShaderRead
      layout_transition(
        cmd,
        m_current_layout,
        vk::ImageLayout::eShaderReadOnlyOptimal,
        vk::AccessFlagBits::eTransferRead,
        vk::AccessFlagBits::eShaderRead,
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eFragmentShader);
    }

    // load to host data
    staging.load(0, std::min(buff_size, dst.size()), dst.data());
  }

} // namespace yave::ui