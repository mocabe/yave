//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/lib/vulkan/vulkan_util.hpp>
#include <yave/lib/vulkan/staging.hpp>
#include <yave/lib/vulkan/texture.hpp>

namespace yave::vulkan {

  /// Offscreen frame buffer data
  struct offscreen_frame_data
  {
    /// extent
    vk::Extent2D extent;
    /// format
    vk::Format format;
    /// memory size
    vk::DeviceSize size;
    /// image
    vk::UniqueImage image;
    /// image memory
    vk::UniqueDeviceMemory memory;
    /// image view
    vk::UniqueImageView view;
    /// frame buffer
    vk::UniqueFramebuffer buffer;
  };

  /// Create offscreen frame
  [[nodiscard]] auto create_offscreen_frame_data(
    const vk::Extent2D& extent,
    const vk::Format& format,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::RenderPass& renderPass,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice) -> offscreen_frame_data;

  /// Clear offscreen frame
  void clear_offscreen_frame_data(
    offscreen_frame_data& dst,
    const vk::ClearColorValue& clearColor,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice);

  /// Upload image data from host
  void store_offscreen_frame_data(
    staging_buffer& staging,
    offscreen_frame_data& dst,
    const vk::Offset2D& offset,
    const vk::Extent2D& size,
    const std::byte* data,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice);

  /// Load image data to host
  void load_offscreen_frame_data(
    staging_buffer& staging,
    const offscreen_frame_data& src,
    const vk::Offset2D& offset,
    const vk::Extent2D& extent,
    std::byte* dstData,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice);

  /// Store texture data to frame
  void store_offscreen_frame_data(
    const texture_data& src,
    const vk::Offset2D& srcOffset,
    offscreen_frame_data& dst,
    const vk::Offset2D& dstOffset,
    const vk::Extent2D& size,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice);

  /// Load frame to texture
  void load_offscreen_frame_data(
    const offscreen_frame_data& src,
    const vk::Offset2D& srcOffset,
    texture_data& dst,
    const vk::Offset2D& dstOffset,
    const vk::Extent2D& size,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice);

} // namespace yave::vulkan