//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/lib/image/image_format.hpp>
#include <yave/lib/buffer/buffer_pool.hpp>
#include <yave/support/id.hpp>
#include <yave/support/uuid.hpp>

#include <vector>
#include <mutex>

namespace yave {

  /// Frame buffer pool interface
  struct frame_buffer_pool : buffer_pool
  {
  protected:
    using buffer_pool::create;

  public:
    frame_buffer_pool(
      void* handle,
      uuid backend_id,
      auto (*create)(void*, uint64_t) noexcept->uid,
      auto (*create_from)(void*, uid) noexcept->uid,
      void (*ref)(void*, uid) noexcept,
      void (*unref)(void*, uid) noexcept,
      auto (*get_use_count)(void* handle, uid id) noexcept->uint64_t,
      auto (*get_data)(void*, uid) noexcept->uint8_t*,
      auto (*get_size)(void*, uid) noexcept->uint64_t,
      auto (*create_fb)(void*) noexcept->uid,
      auto (*get_format)(void*) noexcept->image_format,
      auto (*get_width)(void*) noexcept->uint32_t,
      auto (*get_height)(void*) noexcept->uint32_t,
      auto (*get_byte_size)(void*) noexcept->uint64_t)
      : buffer_pool(
          handle,
          backend_id,
          create,
          create_from,
          ref,
          unref,
          get_use_count,
          get_data,
          get_size)
      , m_create_fb {create_fb}
      , m_get_format {get_format}
      , m_get_width {get_width}
      , m_get_height {get_height}
      , m_get_byte_size {get_byte_size}
    {
    }

    /// Create new buffer
    [[nodiscard]] auto create() const noexcept -> uid
    {
      return m_create_fb(m_handle);
    }

    /// Get image format
    [[nodiscard]] auto format() const noexcept -> image_format
    {
      return m_get_format(m_handle);
    }

    /// Get width
    [[nodiscard]] auto width() const noexcept -> uint32_t
    {
      return m_get_width(m_handle);
    }

    /// Get height
    [[nodiscard]] auto height() const noexcept -> uint32_t
    {
      return m_get_height(m_handle);
    }

    /// Get byte size of format
    [[nodiscard]] auto byte_size() const noexcept -> uint64_t
    {
      return m_get_byte_size(m_handle);
    }

  protected:
    /// Create new frame buffer
    /// \note Should always create buffer of same size, format, etc.
    /// \note Should return 0 when allocation failed.
    auto (*m_create_fb)(void* handle) noexcept -> uid;

    /// Get format of frame buffer.
    /// \note Should always return same format.
    auto (*m_get_format)(void* handle) noexcept -> image_format;

    /// Get width of frame buffer.
    /// \note Should return same value to current frame buffer size.
    auto (*m_get_width)(void* handle) noexcept -> uint32_t;

    /// Get height of frame buffer.
    /// \note Should return same value to current frame buffer size.
    auto (*m_get_height)(void* handle) noexcept -> uint32_t;

    /// Get byte size of frame buffer.
    /// \note Should return same value to cuurent frame buffer size.
    auto (*m_get_byte_size)(void* handle) noexcept -> uint64_t;
  };
} // namespace yave