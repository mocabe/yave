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

  texture::texture(
    u32 width,
    u32 height,
    vk::Format format,
    vulkan_device& device,
    vulkan_allocator& allocator)
  {
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

      allocator.device().createImageViewUnique(info);
    }

    {
      auto cmd = vulkan::single_time_command(
        device.device(),
        device.graphics_queue(),
        device.graphics_command_pool());

      auto cbuff = cmd.command_buffer();
      auto range = imageSubresourceRange();

      // layout   : Undefined -> ShaderReadOnlyOptimal
      // ownership: ??? -> Graphics
      auto barrier = vk::ImageMemoryBarrier()
                       .setImage(m_image.image())
                       .setSubresourceRange(range)
                       .setOldLayout(m_current_layout)
                       .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                       .setSrcQueueFamilyIndex(m_current_queue_family)
                       .setDstQueueFamilyIndex(device.graphics_queue_family())
                       .setDstAccessMask(vk::AccessFlagBits::eShaderRead);

      cbuff.pipelineBarrier(
        vk::PipelineStageFlagBits::eTopOfPipe,
        vk::PipelineStageFlagBits::eFragmentShader,
        {},
        {},
        {},
        barrier);
    }
  }

  void texture::clear_color(
    const vk::ClearColorValue& color,
    vulkan_device& device)
  {
    auto stc = vulkan::single_time_command(
      device.device(), device.graphics_queue(), device.graphics_command_pool());

    auto cmd   = stc.command_buffer();
    auto range = imageSubresourceRange();

    {
      // layout   : ShaderRead -> Transfer
      // owhership: Graphics -> Graphics
      auto barrier = vk::ImageMemoryBarrier()
                       .setImage(m_image.image())
                       .setSubresourceRange(range)
                       .setOldLayout(m_current_layout)
                       .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
                       .setSrcQueueFamilyIndex(m_current_queue_family)
                       .setDstQueueFamilyIndex(device.graphics_queue_family())
                       .setSrcAccessMask(vk::AccessFlagBits::eShaderRead)
                       .setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

      cmd.pipelineBarrier(
        vk::PipelineStageFlagBits::eFragmentShader,
        vk::PipelineStageFlagBits::eTransfer,
        {},
        {},
        {},
        barrier);
    }

    {
      // requires graphics support for queue family
      cmd.clearColorImage(
        m_image.image(), vk::ImageLayout::eTransferDstOptimal, color, range);
    }

    {
      // layout   : Transfer -> ShaderRead
      // owhership: Graphics -> Grahpics
      auto barrier = vk::ImageMemoryBarrier()
                       .setImage(m_image.image())
                       .setSubresourceRange(range)
                       .setOldLayout(m_current_layout)
                       .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                       .setSrcQueueFamilyIndex(m_current_queue_family)
                       .setDstQueueFamilyIndex(device.graphics_queue_family())
                       .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                       .setDstAccessMask(vk::AccessFlagBits::eShaderRead);

      cmd.pipelineBarrier(
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eFragmentShader,
        {},
        {},
        {},
        barrier);
    }
  }

} // namespace yave::ui