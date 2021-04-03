//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/ui/vulkan_device.hpp>
#include <vk_mem_alloc.h>

namespace yave::ui {

  class vulkan_allocator;

  /// Low-level buffer data
  class vulkan_buffer
  {
    VmaAllocator m_allocator      = {};
    VmaAllocation m_allocation    = {};
    VkBuffer m_buffer             = {};
    VkMemoryPropertyFlags m_flags = {};
    VkDeviceSize m_size           = {};
    u8* m_mapped_data             = {};

    friend class vulkan_allocator;

    void destroy()
    {
      if (m_allocator)
        vmaDestroyBuffer(m_allocator, m_buffer, m_allocation);
    }

  public:
    vulkan_buffer()                     = default;
    vulkan_buffer(const vulkan_buffer&) = delete;

    vulkan_buffer(vulkan_buffer&& other) noexcept
    {
      destroy();
      m_allocator   = std::exchange(other.m_allocator, nullptr);
      m_allocation  = std::exchange(other.m_allocation, nullptr);
      m_buffer      = std::exchange(other.m_buffer, nullptr);
      m_flags       = std::exchange(other.m_flags, 0);
      m_size        = std::exchange(other.m_size, 0);
      m_mapped_data = std::exchange(other.m_mapped_data, nullptr);
    }

    void swap(vulkan_buffer& other) noexcept
    {
      std::swap(m_allocator, other.m_allocator);
      std::swap(m_allocation, other.m_allocation);
      std::swap(m_buffer, other.m_buffer);
      std::swap(m_flags, other.m_flags);
      std::swap(m_size, other.m_size);
      std::swap(m_mapped_data, other.m_mapped_data);
    }

    vulkan_buffer& operator=(vulkan_buffer&& other) noexcept
    {
      vulkan_buffer tmp = std::move(other);
      swap(tmp);
      return *this;
    }

    ~vulkan_buffer() noexcept
    {
      destroy();
    }

    bool empty() const
    {
      return m_allocator == nullptr;
    }

    auto allocator() const
    {
      return m_allocator;
    }

    auto allocation() const
    {
      return m_allocation;
    }

    auto buffer() const
    {
      return vk::Buffer(m_buffer);
    }

    auto flags() const
    {
      return vk::MemoryPropertyFlags(m_flags);
    }

    auto alloc_size() const
    {
      return m_size;
    }

    bool is_host_visible() const
    {
      return static_cast<bool>(
        flags() & vk::MemoryPropertyFlagBits::eHostVisible);
    }

    bool is_device_local() const
    {
      return static_cast<bool>(
        flags() & vk::MemoryPropertyFlagBits::eDeviceLocal);
    }

    /// Get host visible pointer of mapped memory range.
    /// \note User must property invalidate/flush memory range on use.
    /// \requires is_host_visible()
    auto mapped_data() -> u8*
    {
      assert(is_host_visible());
      return m_mapped_data;
    }

    /// Get host visible pointer of mapped memory range.
    /// \note User must property invalidate/flush memory range on use.
    /// \requires is_host_visible()
    auto mapped_data() const -> const u8*
    {
      assert(is_host_visible());
      return m_mapped_data;
    }

    /// Invalidate range of mapped memory.
    /// This should be called before reading from memory range to make it
    /// updated.
    /// \requires is_host_visible()
    void invalidate(vk::DeviceSize offset, vk::DeviceSize size) const
    {
      assert(is_host_visible() && m_mapped_data);
      vmaInvalidateAllocation(m_allocator, m_allocation, offset, size);
    }

    /// Flush range of mapped memory.
    /// This should be called after writing to memory range to make it visible
    /// from other memory.
    /// \requires is_host_visible()
    void flush(vk::DeviceSize offset, vk::DeviceSize size)
    {
      assert(is_host_visible() && m_mapped_data);
      vmaFlushAllocation(m_allocator, m_allocation, offset, size);
    }

    /// Invalidate and read memory range
    /// \requires is_host_visible()
    void load(vk::DeviceSize offset, vk::DeviceSize size, u8* dst) const
    {
      assert(offset + size <= m_size);
      invalidate(offset, size);
      std::memcpy(dst, m_mapped_data + offset, size);
    }

    /// Write and flush memory range
    /// \requires is_host_visible()
    void store(vk::DeviceSize offset, vk::DeviceSize size, const u8* src)
    {
      assert(offset + size <= m_size);
      std::memcpy(m_mapped_data + offset, src, size);
      flush(offset, size);
    }
  };

  /// Low-level image data
  class vulkan_image
  {
    VmaAllocator m_allocator      = {};
    VmaAllocation m_allocation    = {};
    VkImage m_image               = {};
    VkMemoryPropertyFlags m_flags = {};
    VkDeviceSize m_size           = {};
    u8* m_mapped_data             = {};

    friend class vulkan_allocator;

    void destroy()
    {
      if (m_allocator)
        vmaDestroyImage(m_allocator, m_image, m_allocation);
    }

  public:
    vulkan_image()                    = default;
    vulkan_image(const vulkan_image&) = delete;

    vulkan_image(vulkan_image&& other) noexcept
    {
      destroy();
      m_allocator   = std::exchange(other.m_allocator, {});
      m_allocation  = std::exchange(other.m_allocation, {});
      m_image       = std::exchange(other.m_image, {});
      m_flags       = std::exchange(other.m_flags, {});
      m_size        = std::exchange(other.m_size, {});
      m_mapped_data = std::exchange(other.m_mapped_data, {});
    }

    void swap(vulkan_image& other) noexcept
    {
      std::swap(m_allocator, other.m_allocator);
      std::swap(m_allocation, other.m_allocation);
      std::swap(m_image, other.m_image);
      std::swap(m_flags, other.m_flags);
      std::swap(m_size, other.m_size);
      std::swap(m_mapped_data, other.m_mapped_data);
    }

    vulkan_image& operator=(vulkan_image&& other) noexcept
    {
      vulkan_image tmp = std::move(other);
      swap(tmp);
      return *this;
    }

    ~vulkan_image() noexcept
    {
      destroy();
    }

    bool empty() const
    {
      return m_allocator == nullptr;
    }

    auto allocator() const
    {
      return m_allocator;
    }

    auto allocation() const
    {
      return m_allocation;
    }

    auto image() const
    {
      return vk::Image(m_image);
    }

    auto flags() const
    {
      return vk::MemoryPropertyFlags(m_flags);
    }

    auto alloc_size() const
    {
      return vk::DeviceSize(m_size);
    }

    bool is_host_visible() const
    {
      return static_cast<bool>(
        flags() & vk::MemoryPropertyFlagBits::eHostVisible);
    }

    bool is_device_local() const
    {
      return static_cast<bool>(
        flags() & vk::MemoryPropertyFlagBits::eDeviceLocal);
    }

    /// Get host visible pointer of mapped memory range.
    /// \note User must property invalidate/flush memory range on use.
    /// \requires is_host_visible()
    auto mapped_data() -> u8*
    {
      assert(is_host_visible());
      return m_mapped_data;
    }

    /// Get host visible pointer of mapped memory range.
    /// \note User must property invalidate/flush memory range on use.
    /// \requires is_host_visible()
    auto mapped_data() const -> const u8*
    {
      assert(is_host_visible());
      return m_mapped_data;
    }

    /// Invalidate range of mapped memory.
    /// This should be called before reading from memory range to make it
    /// updated.
    /// \requires is_host_visible()
    void invalidate(vk::DeviceSize offset, vk::DeviceSize size) const
    {
      assert(is_host_visible() && m_mapped_data);
      vmaInvalidateAllocation(m_allocator, m_allocation, offset, size);
    }

    /// Flush range of mapped memory.
    /// This should be called after writing to memory range to make it visible
    /// from other memory.
    /// \requires is_host_visible()
    void flush(vk::DeviceSize offset, vk::DeviceSize size)
    {
      assert(is_host_visible() && m_mapped_data);
      vmaFlushAllocation(m_allocator, m_allocation, offset, size);
    }

    /// Invalidate and read memory range
    /// \requires is_host_visible()
    void load(vk::DeviceSize offset, vk::DeviceSize size, u8* dst) const
    {
      invalidate(offset, size);
      std::memcpy(dst, m_mapped_data + offset, size);
    }

    /// Write and flush memory range
    /// \requires is_host_visible()
    void store(vk::DeviceSize offset, vk::DeviceSize size, const u8* src)
    {
      std::memcpy(m_mapped_data + offset, src, size);
      flush(offset, size);
    }
  };

  /// Vulkan memory allocator
  class vulkan_allocator
  {
    vulkan_device& m_device;
    VmaAllocator m_allocator;

  public:
    vulkan_allocator(vulkan_device& device);
    vulkan_allocator(const vulkan_allocator&) = delete;
    ~vulkan_allocator() noexcept;

    auto& device()
    {
      return m_device;
    }

    auto& device() const
    {
      return m_device;
    }

  public:
    /// allocate buffer
    auto create_buffer(vk::BufferCreateInfo info, VmaMemoryUsage usage)
      -> vulkan_buffer;
    /// allocate image
    auto create_image(vk::ImageCreateInfo info, VmaMemoryUsage usage)
      -> vulkan_image;
  };

} // namespace yave::ui