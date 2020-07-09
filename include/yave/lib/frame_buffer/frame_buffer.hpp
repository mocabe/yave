//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/lib/image/image_view.hpp>
#include <yave/obj/frame_buffer/frame_buffer_pool.hpp>
#include <yave/support/id.hpp>

namespace yave {

  /// Frame buffer object value.
  struct frame_buffer
  {
    /// Initialize new buffer
    frame_buffer(object_ptr<const FrameBufferPool> pool)
      : m_pool {std::move(pool)}
      , m_id {m_pool->create()}
    {
      if (m_id == 0)
        throw std::runtime_error("Failed to create frame buffer");
    }

    /// deleted
    frame_buffer() = delete;
    /// deleted
    frame_buffer(frame_buffer&&) = delete;

    /// Copy Ctor.
    frame_buffer(const frame_buffer& other)
      : m_pool {other.m_pool}
      , m_id {other.m_pool->create_from(other.m_id)}
    {
      if (m_id == 0)
        throw std::runtime_error("Fialed to create frame buffer");
    }

    /// Dtor.
    /// Decrements refcount.
    ~frame_buffer() noexcept
    {
      m_pool->unref(m_id);
    }

    /// Get current use count
    [[nodiscard]] auto use_count() const -> uint64_t
    {
      return m_pool->get_use_count(m_id);
    }

    /// Get ID of buffer
    [[nodiscard]] auto id() const -> uid
    {
      return {m_id};
    }

    /// Get buffer pool object
    [[nodiscard]] auto pool() const -> object_ptr<const FrameBufferPool>
    {
      return m_pool;
    }

    /// Store data to frame buffer
    void store_data(
      uint32_t offset_x,
      uint32_t offset_y,
      uint32_t width,
      uint32_t height,
      const uint8_t* data)
    {
      m_pool->store_data(m_id, offset_x, offset_y, width, height, data);
    }

    /// Read data from frame buffer
    void read_data(
      uint32_t offset_x,
      uint32_t offset_y,
      uint32_t width,
      uint32_t height,
      uint8_t* data) const
    {
      m_pool->read_data(m_id, offset_x, offset_y, width, height, data);
    }

    /// Get width
    [[nodiscard]] auto width() const
    {
      return m_pool->width();
    }

    /// Get height
    [[nodiscard]] auto height() const
    {
      return m_pool->height();
    }

    /// Get format
    [[nodiscard]] auto format() const
    {
      return m_pool->format();
    }

    /// Get byte size of buffer
    [[nodiscard]] auto byte_size() const
    {
      return const_image_view(nullptr, width(), height(), format()).byte_size();
    }

  private:
    const object_ptr<const FrameBufferPool> m_pool;
    const uint64_t m_id;
  };
} // namespace yave