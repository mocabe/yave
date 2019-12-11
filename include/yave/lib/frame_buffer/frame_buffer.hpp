//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/lib/image/image_view.hpp>
#include <yave/lib/buffer/buffer.hpp>
#include <yave/obj/frame_buffer/frame_buffer_pool.hpp>
#include <yave/support/id.hpp>

namespace yave {

  /// Frame buffer object value.
  struct frame_buffer
  {
    /// Initialize new buffer
    frame_buffer(const object_ptr<FrameBufferPool>& pool)
      : m_pool {pool}
      , m_id {pool->create()}
    {
      if (m_id == 0)
        throw std::bad_alloc();
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
        throw std::bad_alloc();
    }

    /// Dtor.
    /// Decrements refcount.
    ~frame_buffer() noexcept
    {
      m_pool->unref(m_id);
    }

    /// Get data pointer
    [[nodiscard]] auto data() -> uint8_t*
    {
      return m_pool->get_data(m_id);
    }

    /// Get data pointer
    [[nodiscard]] auto data() const -> const uint8_t*
    {
      return m_pool->get_data(m_id);
    }

    /// Get buffer size
    [[nodiscard]] auto size() const -> uint64_t
    {
      return m_pool->get_size();
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
    [[nodiscard]] auto pool() const -> object_ptr<FrameBufferPool>
    {
      return m_pool;
    }

    /// Get image view.
    [[nodiscard]] auto get_image_view() -> mutable_image_view
    {
      return mutable_image_view(
        m_pool->get_data(m_id),
        m_pool->width(),
        m_pool->height(),
        m_pool->format());
    }

    /// Get image view.
    [[nodiscard]] auto get_image_view() const -> const_image_view
    {
      return const_image_view(
        m_pool->get_data(m_id),
        m_pool->width(),
        m_pool->height(),
        m_pool->format());
    }

  private:
    object_ptr<FrameBufferPool> m_pool;
    uint64_t m_id;
  };
} // namespace yave