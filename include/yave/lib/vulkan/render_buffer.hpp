//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>
#include <yave/lib/vulkan/vulkan_util.hpp>

namespace yave::vulkan {

  /// Buffer data for rendering
  struct render_buffer
  {
    /// buffer usage e.g. Vertex or Fragment
    vk::BufferUsageFlags usage;
    /// current size of buffer
    vk::DeviceSize size;
    /// total capacity of buffer
    vk::DeviceSize capacity;
    /// buffer
    vk::UniqueBuffer buffer;
    /// buffer memory
    vk::UniqueDeviceMemory memory;
  };

  /// Create new render buffer
  [[nodiscard]] auto create_render_buffer(
    const vk::BufferUsageFlagBits& usg,
    const vk::DeviceSize& size,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice) -> render_buffer;

  /// Resize render buffer
  void resize_render_buffer(
    render_buffer& buff,
    const vk::DeviceSize& newSize,
    const vk::Device& device,
    const vk::PhysicalDevice& physicalDevice);

  /// Map buffer
  [[nodiscard]] auto map_render_buffer(
    render_buffer& buff,
    const vk::Device& device) -> std::byte*;

  /// Unmap buffer
  void unmap_render_buffer(render_buffer& buff, const vk::Device& device);

} // namespace yave::vulkan