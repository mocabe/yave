//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/obj/buffer/buffer_pool.hpp>
#include <yave/support/id.hpp>

namespace yave {

  /// CRTP base class for buffers
  template <class Derived, class TBufferPool>
  struct buffer_base
  {
    /// deleted
    buffer_base() = delete;
    /// deleted
    buffer_base(buffer_base&&) = delete;

    /// Initialize buffer pool.
    /// \note This constructor does not initialize m_id to allow customization
    /// in Derived class. Be careful.
    explicit buffer_base(const object_ptr<TBufferPool>& pool)
      : m_pool {pool}
      , m_id {} // !!!
    {
    }

    /// Copy Ctor
    buffer_base(const buffer_base& other)
      : m_pool {other.m_pool}
      , m_id {other.m_id}
    {
      m_pool->ref(m_id);
    }

    /// Dtor
    ~buffer_base() noexcept
    {
      m_pool->unref(m_id);
    }

    /// Acquire new buffer object
    auto copy() const -> object_ptr<Box<Derived>>
    {
      return make_object<Box<Derived>>(m_pool, m_pool->create_from(m_id));
    }

    /// Get data pointer
    auto data() -> uint8_t*
    {
      return m_pool->get_data(m_id);
    }

    /// Get data pointer
    auto data() const -> const uint8_t*
    {
      return m_pool->get_data(m_id);
    }

    /// Get buffer size
    auto size() const -> uint64_t
    {
      return m_pool->get_size(m_id);
    }

    /// Get current use count
    auto use_count() const -> uint64_t
    {
      return m_pool->get_use_count(m_id);
    }

    /// Get ID of buffer
    auto id() const -> uid
    {
      return m_id;
    }

    /// Get buffer pool object
    auto pool() const -> object_ptr<TBufferPool>
    {
      return m_pool;
    }

  protected:
    object_ptr<TBufferPool> m_pool;
    uid m_id;
  };

  /// General buffer object value.
  struct buffer : buffer_base<buffer, BufferPool>
  {
    /// Initialize new buffer
    buffer(const object_ptr<BufferPool>& pool, uint64_t size)
      : buffer_base(pool)
    {
      m_id = pool->create(size);

      if (m_id == uid())
        throw std::bad_alloc();
    }

    /// Copy initialize from existing buffer ID
    buffer(const object_ptr<BufferPool>& pool, uid id)
      : buffer_base(pool)
    {
      m_id = m_pool->create_from(id);

      if (m_id == uid())
        throw std::bad_alloc();
    }
  };
}