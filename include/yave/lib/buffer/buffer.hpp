//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/obj/buffer/buffer_pool.hpp>
#include <yave/support/id.hpp>

namespace yave {

  /// buffer handler
  struct buffer
  {
    /// Initialize new buffer
    buffer(const object_ptr<BufferPool>& pool, uint64_t size)
      : m_pool {pool}
      , m_id {pool->create(size)}
    {
      if (m_id == 0)
        throw std::bad_alloc();
    }

    /// deleted
    buffer() = delete;
    /// deleted
    buffer(buffer&&) = delete;

    /// Copy Ctor.
    buffer(const buffer& other)
      : m_pool {other.m_pool}
      , m_id {other.m_pool->create_from(other.m_id)}
    {
      if (m_id == 0)
        throw std::bad_alloc();
    }

    /// Dtor.
    /// Decrements refcount.
    ~buffer() noexcept
    {
      m_pool->unref(m_id);
    }

    /// Get data pointer
    [[nodiscard]] auto data() -> std::byte*
    {
      return m_pool->get_data(m_id);
    }

    /// Get data pointer
    [[nodiscard]] auto data() const -> const std::byte*
    {
      return m_pool->get_data(m_id);
    }

    /// Get buffer size
    [[nodiscard]] auto size() const -> uint64_t
    {
      return m_pool->get_size(m_id);
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
    [[nodiscard]] auto pool() const -> object_ptr<BufferPool>
    {
      return m_pool;
    }

  private:
    object_ptr<BufferPool> m_pool;
    uint64_t m_id;
  };

} // namespace yave