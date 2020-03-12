//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/lib/image/image_format.hpp>
#include <yave/support/uuid.hpp>

namespace yave {

  /// Image buffer pool interface
  struct image_buffer_pool
  {
  public:
    image_buffer_pool(
      void* handle,
      uuid backend_id,
      auto (*create)(void*, uint32_t, uint32_t, image_format) noexcept->uint64_t,
      auto (*create_from)(void*, uint64_t) noexcept->uint64_t,
      void (*ref)(void*, uint64_t) noexcept,
      void (*unref)(void*, uint64_t) noexcept,
      auto (*get_use_count)(void* handle, uint64_t id) noexcept->uint64_t,
      auto (*get_data)(void*, uint64_t) noexcept->std::byte*,
      auto (*get_width)(void*, uint64_t) noexcept->uint32_t,
      auto (*get_height)(void*, uint64_t) noexcept->uint32_t,
      auto (*get_format)(void*, uint64_t) noexcept->image_format,
      auto (*get_native_handle)(void*, uint64_t) noexcept -> uint64_t)
      : m_handle {handle}
      , m_backend_id {backend_id}
      , m_create {create}
      , m_create_from {create_from}
      , m_ref {ref}
      , m_unref {unref}
      , m_get_use_count {get_use_count}
      , m_get_data {get_data}
      , m_get_width {get_width}
      , m_get_height {get_height}
      , m_get_format {get_format}
      , m_get_native_handle {get_native_handle}
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
    [[nodiscard]] auto create(
      uint32_t width,
      uint32_t height,
      image_format format) const noexcept -> uint64_t
    {
      return m_create(m_handle, width, height, format);
    }

    /// Create new buffer from old buffer
    [[nodiscard]] auto create_from(uint64_t id) const noexcept
    {
      return m_create_from(m_handle, id);
    }

    /// Increment reference count of buffer
    [[nodiscard]] auto ref(uint64_t id) const noexcept
    {
      return m_ref(m_handle, id);
    }

    /// Decrement reference count of buffer
    [[nodiscard]] auto unref(uint64_t id) const noexcept
    {
      return m_unref(m_handle, id);
    }

    /// Get use count
    [[nodiscard]] auto use_count(uint64_t id) const noexcept
    {
      return m_get_use_count(m_handle, id);
    }

    /// Access to data of buffer
    [[nodiscard]] auto data(uint64_t id) const noexcept
    {
      return m_get_data(m_handle, id);
    }

    /// Get image format
    [[nodiscard]] auto format(uint64_t id) const noexcept -> image_format
    {
      return m_get_format(m_handle, id);
    }

    /// Get width
    [[nodiscard]] auto width(uint64_t id) const noexcept -> uint32_t
    {
      return m_get_width(m_handle, id);
    }

    /// Get height
    [[nodiscard]] auto height(uint64_t id) const noexcept -> uint32_t
    {
      return m_get_height(m_handle, id);
    }

    /// Get native handle
    [[nodiscard]] auto native_handle(uint64_t id) const noexcept -> uint64_t
    {
      return m_get_native_handle(m_handle, id);
    }

  protected: /* buffer pool info */
    /// Handle to memory manager
    void* m_handle;
    /// Backend ID
    uuid m_backend_id;

  private:
    /// Create new image
    /// \note Should return 0 when construction failed.
    /// \note Should return 0 for unsupoprted formats.
    auto (*m_create)(
      void* handle,
      uint32_t width,
      uint32_t height,
      image_format format) noexcept -> uint64_t;

    /// Create new frame buffer cpyied from exising buffer.
    /// \note Should return 0 when construction failed.
    /// \note id should valid ID for an image allocated with the handle.
    auto (*m_create_from)(void* handle, uint64_t id) noexcept -> uint64_t;

    /// Increment refcount
    /// \note id should valid ID for an image allocated with the handle.
    void (*m_ref)(void* handle, uint64_t id) noexcept;

    /// Decrement reference count.
    /// \note Releases memory when refcount became 0.
    /// \note id should valid ID for an image allocated with the handle.
    void (*m_unref)(void* handle, uint64_t id) noexcept;

    /// Get current use count.
    /// \note id should valid ID for an image allocated with the handle.
    auto (*m_get_use_count)(void* handle, uint64_t id) noexcept -> uint64_t;

    /// Get data pointer.
    /// \note id should valid ID for an image allocated with the handle.
    auto (*m_get_data)(void* handle, uint64_t) noexcept -> std::byte*;

    /// Get width of frame buffer.
    /// \note id should valid ID for an image allocated with the handle.
    auto (*m_get_width)(void* handle, uint64_t) noexcept -> uint32_t;

    /// Get height of frame buffer.
    /// \note id should valid ID for an image allocated with the handle.
    auto (*m_get_height)(void* handle, uint64_t) noexcept -> uint32_t;

    /// Get format of frame buffer.
    /// \note id should valid ID for an image allocated with the handle.
    auto (*m_get_format)(void* handle, uint64_t) noexcept -> image_format;

    /// Get backend specific native handle for the image.
    /// That can be, for example, GPU image handler which can be used to
    /// optimize memory transfer between CPU and GPU.
    /// \note this function can return 0 when there's no native handle avalable,
    /// then the image is already avalable from CPU visible memory area so use
    /// get_data() to access it.
    /// \note behaviour of this function is highly backend specific.
    auto (*m_get_native_handle)(void* handle, uint64_t) noexcept -> uint64_t;
  };
} // namespace yave