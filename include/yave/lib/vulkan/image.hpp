//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>
#include <yave/lib/vulkan/vulkan_util.hpp>

namespace yave::vulkan {

  /// get planer image subresourec range
  [[nodiscard]]
  inline auto image_subresource_range() -> vk::ImageSubresourceRange
  {
    vk::ImageSubresourceRange range;
    range.aspectMask = vk::ImageAspectFlagBits::eColor;
    range.layerCount = 1;
    range.levelCount = 1;
    return range;
  }

  /// change planar image layout
  inline void image_layout_transition(
    vk::CommandBuffer cmd,
    vk::Image image,
    vk::ImageLayout oldLayout,
    vk::ImageLayout newLayout,
    vk::AccessFlags srcAccessMask,
    vk::AccessFlags dstAccessMask,
    vk::PipelineStageFlags srcStage,
    vk::PipelineStageFlags dstStage)
  {
    auto range = image_subresource_range();

    vk::ImageMemoryBarrier barrier;
    barrier.image            = image;
    barrier.subresourceRange = range;
    barrier.oldLayout        = oldLayout;
    barrier.newLayout        = newLayout;
    barrier.srcAccessMask    = srcAccessMask;
    barrier.dstAccessMask    = dstAccessMask;

    cmd.pipelineBarrier(srcStage, dstStage, {}, {}, {}, barrier);
  }

  /// ShaderReadOnly -> TransferDstOptimal
  inline void image_layout_shader_read_to_transfer_dst(
    const vk::CommandBuffer& cmd,
    const vk::Image& image)
  {
    image_layout_transition(
      cmd,
      image,
      vk::ImageLayout::eShaderReadOnlyOptimal,
      vk::ImageLayout::eTransferDstOptimal,
      vk::AccessFlagBits::eShaderRead,
      vk::AccessFlagBits::eTransferWrite,
      vk::PipelineStageFlagBits::eFragmentShader,
      vk::PipelineStageFlagBits::eTransfer);
  }

  /// ShaderReadOnly -> TransferSrcOptimal
  inline void image_layout_shader_read_to_transfer_src(
    const vk::CommandBuffer& cmd,
    const vk::Image& image)
  {
    image_layout_transition(
      cmd,
      image,
      vk::ImageLayout::eShaderReadOnlyOptimal,
      vk::ImageLayout::eTransferSrcOptimal,
      vk::AccessFlagBits::eShaderRead,
      vk::AccessFlagBits::eTransferRead,
      vk::PipelineStageFlagBits::eFragmentShader,
      vk::PipelineStageFlagBits::eTransfer);
  }

  /// TransferDst -> ShaderReadOnly
  inline void image_layout_transfer_dst_to_shader_read(
    const vk::CommandBuffer& cmd,
    const vk::Image& image)
  {
    image_layout_transition(
      cmd,
      image,
      vk::ImageLayout::eTransferDstOptimal,
      vk::ImageLayout::eShaderReadOnlyOptimal,
      vk::AccessFlagBits::eTransferWrite,
      vk::AccessFlagBits::eShaderRead,
      vk::PipelineStageFlagBits::eTransfer,
      vk::PipelineStageFlagBits::eFragmentShader);
  }

  /// TransferSrc -> ShaderReadOnly
  inline void image_layout_transfer_src_to_shader_read(
    const vk::CommandBuffer& cmd,
    const vk::Image& image)
  {
    image_layout_transition(
      cmd,
      image,
      vk::ImageLayout::eTransferSrcOptimal,
      vk::ImageLayout::eShaderReadOnlyOptimal,
      vk::AccessFlagBits::eTransferRead,
      vk::AccessFlagBits::eShaderRead,
      vk::PipelineStageFlagBits::eTransfer,
      vk::PipelineStageFlagBits::eFragmentShader);
  }

  /// Undefined -> TransferDst
  inline void image_layout_undefined_to_transfer_dst(
    const vk::CommandBuffer& cmd,
    const vk::Image& image)
  {
    image_layout_transition(
      cmd,
      image,
      vk::ImageLayout::eUndefined,
      vk::ImageLayout::eTransferDstOptimal,
      vk::AccessFlagBits(),
      vk::AccessFlagBits::eTransferWrite,
      vk::PipelineStageFlagBits::eTopOfPipe,
      vk::PipelineStageFlagBits::eTransfer);
  }

  /// Undefined -> ShaderReadOnly
  inline void image_layout_undefined_to_shader_read(
    const vk::CommandBuffer& cmd,
    const vk::Image& image)
  {
    image_layout_transition(
      cmd,
      image,
      vk::ImageLayout::eUndefined,
      vk::ImageLayout::eShaderReadOnlyOptimal,
      vk::AccessFlagBits(),
      vk::AccessFlagBits::eShaderRead,
      vk::PipelineStageFlagBits::eTopOfPipe,
      vk::PipelineStageFlagBits::eFragmentShader);
  }

  /// Undefined -> ColorAttachment
  inline void image_layout_undefined_to_color_attachment(
    const vk::CommandBuffer& cmd,
    const vk::Image& image)
  {
    image_layout_transition(
      cmd,
      image,
      vk::ImageLayout::eUndefined,
      vk::ImageLayout::eColorAttachmentOptimal,
      vk::AccessFlagBits(),
      vk::AccessFlagBits::eColorAttachmentRead
        | vk::AccessFlagBits::eColorAttachmentRead,
      vk::PipelineStageFlagBits::eTopOfPipe,
      vk::PipelineStageFlagBits::eColorAttachmentOutput);
  }

  /// ColorAttachment -> TransferDst
  inline void image_layout_color_attachment_to_transfer_dst(
    const vk::CommandBuffer& cmd,
    const vk::Image& image)
  {
    image_layout_transition(
      cmd,
      image,
      vk::ImageLayout::eColorAttachmentOptimal,
      vk::ImageLayout::eTransferDstOptimal,
      vk::AccessFlagBits::eColorAttachmentRead
        | vk::AccessFlagBits::eColorAttachmentWrite,
      vk::AccessFlagBits::eTransferWrite,
      vk::PipelineStageFlagBits::eColorAttachmentOutput,
      vk::PipelineStageFlagBits::eTransfer);
  }

  /// ColorAttachment -> TransferSrc
  inline void image_layout_color_attachment_to_transfer_src(
    const vk::CommandBuffer& cmd,
    const vk::Image& image)
  {
    image_layout_transition(
      cmd,
      image,
      vk::ImageLayout::eColorAttachmentOptimal,
      vk::ImageLayout::eTransferSrcOptimal,
      vk::AccessFlagBits::eColorAttachmentRead
        | vk::AccessFlagBits::eColorAttachmentWrite,
      vk::AccessFlagBits::eTransferRead,
      vk::PipelineStageFlagBits::eColorAttachmentOutput,
      vk::PipelineStageFlagBits::eTransfer);
  }

  /// TransferDst -> ColorAttachment
  inline void image_layout_transfer_dst_to_color_attachment(
    const vk::CommandBuffer& cmd,
    const vk::Image& image)
  {
    image_layout_transition(
      cmd,
      image,
      vk::ImageLayout::eTransferDstOptimal,
      vk::ImageLayout::eColorAttachmentOptimal,
      vk::AccessFlagBits::eTransferWrite,
      vk::AccessFlagBits::eColorAttachmentRead
        | vk::AccessFlagBits::eColorAttachmentWrite,
      vk::PipelineStageFlagBits::eTransfer,
      vk::PipelineStageFlagBits::eColorAttachmentOutput);
  }

  /// TransferSrc -> ColorAttachment
  inline void image_layout_transfer_src_to_color_attachment(
    const vk::CommandBuffer& cmd,
    const vk::Image& image)
  {
    image_layout_transition(
      cmd,
      image,
      vk::ImageLayout::eTransferSrcOptimal,
      vk::ImageLayout::eColorAttachmentOptimal,
      vk::AccessFlagBits::eTransferRead,
      vk::AccessFlagBits::eColorAttachmentRead
        | vk::AccessFlagBits::eColorAttachmentWrite,
      vk::PipelineStageFlagBits::eTransfer,
      vk::PipelineStageFlagBits::eColorAttachmentOutput);
  }
} // namespace yave::vulkan