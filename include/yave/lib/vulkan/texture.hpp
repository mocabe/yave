//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/lib/vulkan/vulkan_util.hpp>
#include <yave/lib/vulkan/staging.hpp>

namespace yave::vulkan {

  /// 2D Texture data structure for vulkan.
  struct texture_data
  {
    /// extent
    vk::Extent2D extent;
    /// format
    vk::Format format = vk::Format::eUndefined;
    /// size
    vk::DeviceSize size = 0;
    /// texture image.
    /// this image has following specs:
    /// layout:  eShaderReadOnlyOptimal
    /// usage:   eSampled | eTransferDst | eTransferSrc
    /// sharing: eExclusive
    /// range:   single layer, single level
    vk::UniqueImage image;
    /// image view
    vk::UniqueImageView view;
    /// image memory
    vk::UniqueDeviceMemory memory;
  };

  /// Create texture data on GPU.
  /// User should fill data after this to use.
  [[nodiscard]] auto create_texture_data(
    const uint32_t& width,
    const uint32_t& height,
    const vk::Format& format,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice) -> texture_data;

  /// Create new texture by copying existing texture data
  [[nodiscard]] auto clone_texture_data(
    const texture_data& tex,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice) -> texture_data;

  /// Clear texture image
  void clear_texture_data(
    texture_data& dst,
    const vk::ClearColorValue& clearColor,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice);

  /// Write image data to texture from host memory
  /// \param offset offset in texture data to write
  /// \param size size of area of texture to write
  /// \param data host memory to read
  /// \note reads `(size of texel) * size.width * size.height` bytes from `data`
  void store_texture_data(
    staging_buffer& staging,
    texture_data& dst,
    const vk::Offset2D& offset,
    const vk::Extent2D& size,
    const uint8_t* data,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice);

  /// Read back texture data to host memory
  /// \param offset offset in texture data to read
  /// \param size size of area of texture to read
  /// \param data host memory to write
  void load_texture_data(
    staging_buffer& staging,
    const texture_data& src,
    const vk::Offset2D& offset,
    const vk::Extent2D& size,
    uint8_t* data,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice);

} // namespace yave::vulkan