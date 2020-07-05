//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

// https://github.com/KhronosGroup/Vulkan-Hpp/pull/431
#if defined(YAVE_OS_WINDOWS)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif

#include <yave/lib/image/blend_operation.hpp>
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

  /// Find memory type index
  [[nodiscard]] auto find_memory_type_index(
    const vk::MemoryRequirements& requirements,
    const vk::MemoryPropertyFlags& properties,
    const vk::PhysicalDevice& physicalDevice) -> uint32_t;

  /// Get texel size of format
  [[nodiscard]] auto format_texel_size(const vk::Format& format)
    -> vk::DeviceSize;

  /// Get blend state from blend op
  [[nodiscard]] auto convert_to_blend_state(blend_operation op)
    -> vk::PipelineColorBlendAttachmentState;

} // namespace yave