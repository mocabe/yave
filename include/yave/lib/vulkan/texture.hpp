//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <vulkan/vulkan.hpp>

namespace yave::vulkan {

  /// 2D Texture data structure for vulkan.
  struct texture_data
  {
    /// width
    uint32_t width = 0;
    /// height
    uint32_t height = 0;
    /// format
    vk::Format format = vk::Format::eUndefined;
    /// texture image.
    /// this image has following specs:
    /// layout:  eShaderReadOnlyOptimal
    /// usage:   eSampled | eTransferDst
    /// sharing: eExclusive
    /// range:   single layer, single level
    vk::UniqueImage image;
    /// image view
    vk::UniqueImageView view;
    /// image memory
    vk::UniqueDeviceMemory memory;
    /// descriptor set
    vk::UniqueDescriptorSet dsc_set;
  };

  /// Create texture data on GPU.
  /// User should fill data after this to use.
  [[nodiscard]] auto create_texture_data(
    const uint32_t& width,
    const uint32_t& height,
    const vk::Format& format,
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::DescriptorPool& dscPool,
    const vk::DescriptorSetLayout& dscLayout,
    const vk::DescriptorType& dscType,
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

  /// Upload image data to texture from host memory
  void store_texture_data(
    texture_data& dst,        // texture to upload
    const std::byte* srcData, // src data pointer
    const uint32_t& srcSize,  // src data size
    const vk::Queue& cmdQueue,
    const vk::CommandPool& cmdPool,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice);

} // namespace yave::vulkan