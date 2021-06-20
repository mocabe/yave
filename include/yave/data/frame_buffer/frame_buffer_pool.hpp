//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>
#include <yave/lib/image/image_format.hpp>
#include <yave/support/uuid.hpp>

namespace yave::data {

  /// Frame buffer pool interface
  struct frame_buffer_pool
  {
  public:
    frame_buffer_pool(
      void* handle,
      uuid backend_id,
      auto (*create)(void*) noexcept->uint64_t,
      auto (*create_from)(void*, uint64_t) noexcept->uint64_t,
      void (*destroy)(void*, uint64_t) noexcept,
      void (*store_data)(void*, uint64_t ,uint32_t, uint32_t, uint32_t, uint32_t, const uint8_t*) noexcept,
      void (*read_data )(void*, uint64_t, uint32_t, uint32_t, uint32_t, uint32_t, uint8_t*) noexcept,
      auto (*get_width)(void*) noexcept->uint32_t,
      auto (*get_height)(void*) noexcept->uint32_t,
      auto (*get_format)(void*) noexcept->image_format)
      : m_handle {handle}
      , m_backend_id {backend_id}
      , m_create {create}
      , m_create_from {create_from}
      , m_destroy {destroy}
      , m_store_data {store_data}
      , m_read_data {read_data}
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
    [[nodiscard]] auto create() const noexcept
    {
      return m_create(m_handle);
    }

    /// Create new buffer from old buffer
    [[nodiscard]] auto create_from(uint64_t id) const noexcept
    {
      return m_create_from(m_handle, id);
    }

    /// Destroy frame buffer
    [[nodiscard]] auto destroy(uint64_t id) const noexcept
    {
      return m_destroy(m_handle, id);
    }

    /// Store data
    void store_data(
      uint64_t id,
      uint32_t offset_x,
      uint32_t offset_y,
      uint32_t width,
      uint32_t height,
      const uint8_t* data) const noexcept
    {
      m_store_data(m_handle, id, offset_x, offset_y, width, height, data);
    }

    /// Read data
    void read_data(
      uint64_t id,
      uint32_t offset_x,
      uint32_t offset_y,
      uint32_t width,
      uint32_t height,
      uint8_t* data) const noexcept
    {
      m_read_data(m_handle, id, offset_x, offset_y, width, height, data);
    }

    /// Get width
    [[nodiscard]] auto width() const noexcept
    {
      return m_get_width(m_handle);
    }

    /// Get height
    [[nodiscard]] auto height() const noexcept 
    {
      return m_get_height(m_handle);
    }

    /// Get image format
    [[nodiscard]] auto format() const noexcept 
    {
      return m_get_format(m_handle);
    }

  protected: /* buffer pool info */
    /// Handle to buffer pool object
    void* m_handle;
    /// Backend ID
    uuid m_backend_id;

  private:
    /// Create new frame buffer
    /// \note Always creates buffer of same size and format.
    /// \note Should return 0 when allocation failed.
    auto (*m_create)(void* handle) noexcept -> uint64_t;

    /// Create new frame buffer cpyied from exising buffer.
    /// \note Should return 0 when allocation failed.
    /// \note id should be valid.
    auto (*m_create_from)(void* handle, uint64_t id) noexcept -> uint64_t;

    /// Destroy reference count.
    /// \note id should be valid.
    void (*m_destroy)(void* handle, uint64_t id) noexcept;

    /// Store data to buffer
    void (*m_store_data)(
      void* handle,
      uint64_t id,
      uint32_t offset_x,
      uint32_t offset_y,
      uint32_t width,
      uint32_t height,
      const uint8_t* data) noexcept;

    /// Read data from buffer
    void (*m_read_data)(
      void* handle,
      uint64_t id,
      uint32_t offset_x,
      uint32_t offset_y,
      uint32_t width,
      uint32_t height,
      uint8_t* data) noexcept;

    /// Get format of frame buffer.
    /// \note Should always return same format.
    auto (*m_get_format)(void* handle) noexcept -> image_format;

    /// Get width of frame buffer.
    /// \note Should always return same size.
    auto (*m_get_width)(void* handle) noexcept -> uint32_t;

    /// Get height of frame buffer.
    /// \note Should always return same size.
    auto (*m_get_height)(void* handle) noexcept -> uint32_t;
  };
} // namespace yave