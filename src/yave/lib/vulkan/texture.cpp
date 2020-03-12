//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/vulkan/texture.hpp>
#include <yave/lib/vulkan/vulkan_util.hpp>

namespace yave::vulkan {

  namespace {

    auto textureImageSubresourceRange()
    {
      vk::ImageSubresourceRange range;
      {
        range.aspectMask = vk::ImageAspectFlagBits::eColor;
        range.layerCount = 1;
        range.levelCount = 1;
      }
      return range;
    }

    void textureLayoutShaderReadOnlyToTransferDst(
      const vk::CommandBuffer& cmd,
      const vk::Image& image)
    {
      auto range = textureImageSubresourceRange();

      vk::ImageMemoryBarrier barrier;
      barrier.image            = image;
      barrier.subresourceRange = range;
      barrier.oldLayout        = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.newLayout        = vk::ImageLayout::eTransferDstOptimal;
      barrier.srcAccessMask    = vk::AccessFlagBits::eShaderRead;
      barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

      vk::PipelineStageFlags srcStage =
        vk::PipelineStageFlagBits::eFragmentShader;
      vk::PipelineStageFlags dstStage = vk::PipelineStageFlagBits::eTransfer;

      cmd.pipelineBarrier(srcStage, dstStage, {}, {}, {}, barrier);
    }

    void textureLayoutTransferDstToShaderReadOnly(
      const vk::CommandBuffer& cmd,
      const vk::Image& image)
    {
      auto range = textureImageSubresourceRange();

      vk::ImageMemoryBarrier barrier;
      barrier.image            = image;
      barrier.subresourceRange = range;
      barrier.oldLayout        = vk::ImageLayout::eTransferDstOptimal;
      barrier.newLayout        = vk::ImageLayout::eShaderReadOnlyOptimal;
      barrier.srcAccessMask    = vk::AccessFlagBits::eTransferWrite;
      barrier.dstAccessMask    = vk::AccessFlagBits::eShaderRead;

      vk::PipelineStageFlags srcStage = vk::PipelineStageFlagBits::eTransfer;
      vk::PipelineStageFlags dstStage =
        vk::PipelineStageFlagBits::eFragmentShader;

      cmd.pipelineBarrier(srcStage, dstStage, {}, {}, {}, barrier);
    }
  } // namespace

  auto create_texture_data(
    const uint32_t& width,
    const uint32_t& height,
    const vk::Format& format,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::DescriptorPool& dscPool,
    const vk::DescriptorSetLayout& dscLayout,
    const vk::DescriptorType& dscType,
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
      info.usage =
        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
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

    vk::UniqueDescriptorSet dsc;
    {
      vk::DescriptorSetAllocateInfo info;
      info.descriptorPool     = dscPool;
      info.descriptorSetCount = 1;
      info.pSetLayouts        = &dscLayout;
      dsc = std::move(device.allocateDescriptorSetsUnique(info)[0]);
    }

    {
      vk::DescriptorImageInfo info;
      info.imageView   = view.get();
      info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

      vk::WriteDescriptorSet write;
      write.descriptorCount = 1;
      write.dstSet          = dsc.get();
      write.descriptorType  = dscType;
      write.pImageInfo      = &info;
      device.updateDescriptorSets(write, {});
    }

    {
      using namespace yave::vulkan;
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

      textureLayoutTransferDstToShaderReadOnly(cmd, image.get());
    }

    return {width,
            height,
            format,
            format_texel_size(format) * width * height,
            std::move(image),
            std::move(view),
            std::move(memory),
            std::move(dsc)};
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
    const std::byte* srcData,
    const uint32_t& srcSize,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice)
  {
    assert(srcSize <= dst.size);

    resize_staging_buffer(staging, srcSize, device, physicalDevice);

    auto buffer       = staging.buffer.get();
    auto bufferMemory = staging.memory.get();

    /* upload data */
    {
      void* ptr = device.mapMemory(bufferMemory, 0, srcSize);
      std::memcpy(ptr, srcData, srcSize);
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
      region.imageExtent = vk::Extent3D {dst.width, dst.height, 1};

      cmd.copyBufferToImage(
        buffer, dst.image.get(), vk::ImageLayout::eTransferDstOptimal, region);

      textureLayoutTransferDstToShaderReadOnly(cmd, dst.image.get());
    }
  }

} // namespace yave::vulkan