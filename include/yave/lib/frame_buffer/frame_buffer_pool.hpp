//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/lib/image/image_format.hpp>
#include <yave/support/uuid.hpp>

namespace yave {

  /// Frame buffer pool interface
  struct frame_buffer_pool
  {
  public:
    frame_buffer_pool(
      void* handle,
      uuid backend_id,
      auto (*create)(void*) noexcept->uint64_t,
      auto (*create_from)(void*, uint64_t) noexcept->uint64_t,
      void (*ref)(void*, uint64_t) noexcept,
      void (*unref)(void*, uint64_t) noexcept,
      auto (*get_use_count)(void* handle, uint64_t id) noexcept->uint64_t,
      auto (*get_data)(void*, uint64_t) noexcept->std::byte*,
      auto (*get_size)(void*) noexcept->uint64_t,
      auto (*get_format)(void*) noexcept->image_format,
      auto (*get_width)(void*) noexcept->uint32_t,
      auto (*get_height)(void*) noexcept->uint32_t)
      : m_handle {handle}
      , m_backend_id {backend_id}
      , m_create {create}
      , m_create_from {create_from}
      , m_ref {ref}
      , m_unref {unref}
      , m_get_use_count {get_use_count}
      , m_get_data {get_data}
      , m_get_size {get_size}
      , m_get_format {get_format}
      , m_get_width {get_width}
      , m_get_height {get_height}
    {
    }

    /// Get buffer pool handle
    [[nodiscard]] auto handle() const noexcept
    {
      return m_handle;
    }

    /// Get backend ID
    [[nodiscard]] auto backend_id() const noexcept
    {
      return m_backend_id;
    }

    /// Create new buffer
    [[nodiscard]] auto create() const noexcept -> uint64_t
    {
      return m_create(m_handle);
    }

    /// Create new buffer from old buffer
    [[nodiscard]] auto create_from(const uint64_t& id) const noexcept
    {
      return m_create_from(m_handle, id);
    }

    /// Increment reference count of buffer
    [[nodiscard]] auto ref(const uint64_t& id) const noexcept
    {
      return m_ref(m_handle, id);
    }

    /// Decrement reference count of buffer
    [[nodiscard]] auto unref(const uint64_t& id) const noexcept
    {
      return m_unref(m_handle, id);
    }

    /// Get use count
    [[nodiscard]] auto get_use_count(const uint64_t& id) const noexcept
    {
      return m_get_use_count(m_handle, id);
    }

    /// Access to data of buffer
    [[nodiscard]] auto get_data(const uint64_t& id) const noexcept
    {
      return m_get_data(m_handle, id);
    }

    /// Get size of buffer
    [[nodiscard]] auto get_size() const noexcept
    {
      return m_get_size(m_handle);
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

  protected: /* buffer pool info */
    /// Handle to buffer pool object
    void* m_handle;
    /// Backend ID
    uuid m_backend_id;

  private:
    // See buffer_pool's members for more detailed specifications for similar
    // function pointers.

    /// Create new frame buffer
    /// \note Should always create buffer of same size, format, etc.
    auto (*m_create)(void* handle) noexcept -> uint64_t;

    /// Create new frame buffer cpyied from exising buffer.
    /// \note Should return 0 when `id` is invalid.
    /// \note Should return 0 when allocation failed.
    auto (*m_create_from)(void* handle, uint64_t id) noexcept -> uint64_t;

    /// Increment refcount
    void (*m_ref)(void* handle, uint64_t id) noexcept;

    /// Decrement reference count.
    void (*m_unref)(void* handle, uint64_t id) noexcept;

    /// Get current use count.
    auto (*m_get_use_count)(void* handle, uint64_t id) noexcept -> uint64_t;

    /// Get data pointer.
    auto (*m_get_data)(void* handle, uint64_t) noexcept -> std::byte*;

    /// Get byte size of frame buffer.
    /// \note Should return same value to cuurent frame buffer size.
    auto (*m_get_size)(void* handle) noexcept -> uint64_t;

    /// Get format of frame buffer.
    /// \note Should always return same format.
    auto (*m_get_format)(void* handle) noexcept -> image_format;

    /// Get width of frame buffer.
    /// \note Should return same value to current frame buffer size.
    auto (*m_get_width)(void* handle) noexcept -> uint32_t;

    /// Get height of frame buffer.
    /// \note Should return same value to current frame buffer size.
    auto (*m_get_height)(void* handle) noexcept -> uint32_t;
  };
} // namespace yave