//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/vulkan_allocator.hpp>

namespace yave::ui {

  /// Staging memory resource
  class staging_buffer
  {
    /// allocator ref
    vulkan_allocator& m_allocator;
    /// Vulkan buffer object
    vulkan_buffer m_buffer;
    /// create info
    vk::BufferCreateInfo m_create_info;
    /// current size
    vk::DeviceSize m_current_size;

  public:
    // Expected to be used with smart pointers
    staging_buffer(const staging_buffer&) = delete;
    staging_buffer(staging_buffer&&)      = delete;

    /// Create new buffer
    staging_buffer(size_t size, vulkan_allocator& allocator);

    /// Resize buffer
    void resize(size_t new_size);

    /// Shrink to current size
    void shrink_to_fit();

    auto buffer() const
    {
      return m_buffer.buffer();
    }

    auto size() const
    {
      return m_current_size;
    }

    auto capacity() const
    {
      return m_create_info.size;
    }

    bool empty() const
    {
      return size() == 0;
    }

    void store(size_t offset, size_t size, const u8* data)
    {
      m_buffer.store(offset, size, data);
    }

    void load(size_t offset, size_t size, u8* data) const
    {
      m_buffer.load(offset, size, data);
    }
  };

} // namespace yave::ui
