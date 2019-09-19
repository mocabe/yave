//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/rts/object_ptr.hpp>
#include <yave/support/id.hpp>

namespace yave {

  template <class T>
  struct Box; // fwd

  class buffer;       // -> lib/buffer/buffer.hpp
  struct buffer_pool; // -> lib/buffer/buffer_pool.hpp

  using Buffer     = Box<buffer>;      // -> obj/buffer/buffer.hpp
  using BufferPool = Box<buffer_pool>; // -> obj/buffer/buffer_pool.hpp

  /// General buffer object value.
  class buffer
  {
  public:
    /// deleted
    buffer() = delete;
    /// deleted
    buffer(buffer&&) = delete;

    /// Initialize new buffer
    buffer(const object_ptr<BufferPool>& pool, uint64_t size);
    /// Initialize new buffer from exiting buffer
    buffer(const object_ptr<BufferPool>& pool, uid id);

    /// Copy ctor
    buffer(const buffer& other);
    /// Dtor
    ~buffer() noexcept;

    /// Acquire new buffer object
    [[nodiscard]] auto copy() const -> object_ptr<Buffer>;

    /// Get data pointer
    [[nodiscard]] auto data() -> uint8_t*;

    /// Get data pointer
    [[nodiscard]] auto data() const -> const uint8_t*;

    /// Get size of this buffer
    [[nodiscard]] auto size() const -> uint64_t;

    /// Get use count
    [[nodiscard]] auto use_count() const -> uint64_t;

    /// Get ID
    [[nodiscard]] auto id() const -> uid;

    /// Get buffer pool
    [[nodiscard]] auto pool() const -> object_ptr<BufferPool>;

  private:
    object_ptr<BufferPool> m_pool;
    uid m_id;
  };
}