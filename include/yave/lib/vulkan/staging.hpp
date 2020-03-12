//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/vulkan/vulkan_context.hpp>

namespace yave::vulkan {

  struct staging_buffer
  {
    /// Buffer
    /// usage: eTransferSrc | eTransferDst
    vk::UniqueBuffer buffer;
    /// Host visible memory
    /// prop: eHostVisible | eHostCoherent
    vk::UniqueDeviceMemory memory;
    /// Size of memory
    vk::DeviceSize size;
  };

  /// Create new staging buffer
  [[nodiscard]] auto create_staging_buffer(
    const vk::DeviceSize& size,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice) -> staging_buffer;

  /// Resize staging buffer
  void resize_staging_buffer(
    staging_buffer& staging,
    const vk::DeviceSize& size,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice);

} // namespace yave::vulkan