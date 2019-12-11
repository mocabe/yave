//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/lib/image/image_format.hpp>
#include <yave/lib/buffer/buffer_manager.hpp>
#include <yave/obj/frame_buffer/frame_buffer_pool.hpp>
#include <yave/obj/frame_buffer/frame_buffer.hpp>
#include <yave/support/id.hpp>
#include <yave/support/uuid.hpp>

#include <vector>
#include <mutex>

namespace yave {

  /// Simple frame buffer factory base on buffer_manager
  class frame_buffer_manager
  {
  public:
    /// ctor
    frame_buffer_manager(
      uint32_t width,
      uint32_t height,
      const image_format& format,
      const uuid& backend_id = uuid());

    frame_buffer_manager(const frame_buffer_manager&) = delete;
    frame_buffer_manager& operator=(const frame_buffer_manager&) = delete;

    /// move ctor
    frame_buffer_manager(frame_buffer_manager&&) noexcept;
    /// move assign
    frame_buffer_manager& operator=(frame_buffer_manager&&) noexcept;

    /// dtor
    ~frame_buffer_manager() noexcept;

    /// create
    [[nodiscard]] uid create() noexcept;
    /// create from
    [[nodiscard]] uid create_from(uid) noexcept;

    /// ref
    void ref(uid id) noexcept;
    /// unref
    void unref(uid id) noexcept;

    /// Get use count
    [[nodiscard]] auto use_count(uid id) const noexcept -> uint64_t;
    /// Get data pointer
    [[nodiscard]] auto data(uid id) const noexcept -> uint8_t*;
    /// Get byte sautouffer
    [[nodiscard]] auto size() const noexcept -> uint64_t;

    /// Get image format.
    [[nodiscard]] auto format() const noexcept -> image_format;
    /// Get width of buffers.
    [[nodiscard]] auto width() const noexcept -> uint32_t;
    /// Get heightautoers.
    [[nodiscard]] auto height() const noexcept -> uint32_t;

    /// Get proxy data
    [[nodiscard]] auto get_pool_object() const noexcept
      -> object_ptr<FrameBufferPool>;

  private:
    buffer_manager m_mngr;

  private:
    image_format m_format;
    uint32_t m_width;
    uint32_t m_height;

  private:
    // proxy data for backend agnostic frame buffer management.
    object_ptr<FrameBufferPool> m_pool;
  };
}