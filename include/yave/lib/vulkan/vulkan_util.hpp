//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

// https://github.com/KhronosGroup/Vulkan-Hpp/pull/431
#if defined(YAVE_OS_WINDOWS)
#  include <windows.h>
#endif

#include <vulkan/vulkan.hpp>

namespace yave::vulkan {

  /// single time command
  class single_time_command
  {
  public:
    /// Create command buffer and start recording.
    single_time_command(
      const vk::Device& device,
      const vk::Queue& queue,
      const vk::CommandPool& pool);
    /// Move single time command.
    single_time_command(single_time_command&& other) noexcept;
    /// End command buffer and submit.
    ~single_time_command();

  public:
    /// Get recording command buffer.
    [[nodiscard]] auto command_buffer() const -> vk::CommandBuffer;

  private:
    single_time_command(const single_time_command&) = delete;

  private:
    vk::Device m_device;
    vk::Queue m_queue;
    vk::CommandPool m_pool;
    vk::UniqueCommandBuffer m_buffer;
    vk::UniqueFence m_fence;
  };

  /// Upload image to GPU
  [[nodiscard]] auto upload_image(
    const vk::Extent2D& image_extent,
    const vk::DeviceSize& byte_size,
    const vk::Format& format,
    const uint8_t* data,
    const vk::CommandPool& commandPool,
    const vk::Queue queue,
    const vk::PhysicalDevice& physicalDevice,
    const vk::Device& device)
    -> std::tuple<vk::UniqueImage, vk::UniqueImageView, vk::UniqueDeviceMemory>;

  /// Create descriptor set from image view
  [[nodiscard]] auto create_image_descriptor(
    const vk::ImageView& image,
    const vk::DescriptorSetLayout& layout,
    const vk::DescriptorPool& pool,
    const vk::Device& device) -> vk::UniqueDescriptorSet;

} // namespace yave