//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/vulkan_allocator.hpp>

namespace yave::ui {

  /// For vertex and index buffers.
  class render_buffer
  {
    vulkan_allocator& m_allocator;
    vulkan_buffer m_buffer;
    vk::BufferUsageFlags m_usage;
    vk::DeviceSize m_size;
    vk::DeviceSize m_capacity;

  public:
    /// \param usage vertex buffer or index buffer
    render_buffer(vk::BufferUsageFlags usage, vulkan_allocator& allocator);
    render_buffer(const render_buffer&) = delete;
    render_buffer(render_buffer&&)      = delete;

    /// size
    auto size() const
    {
      return m_size;
    }

    /// capacity
    auto capacity() const
    {
      return m_capacity;
    }

    /// Get buffer handle
    auto buffer() const
    {
      return m_buffer.buffer();
    }

  public:
    /// Call this before writing data
    auto begin_write() -> u8*;
    /// Call this after writing data
    void end_write();

  public:
    /// \note Will NOT preserve data after resize.
    void resize(size_t new_size);
  };

} // namespace yave::ui