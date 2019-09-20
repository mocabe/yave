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

  class frame_buffer_manager : public buffer_manager
  {
  public:
    /// ctor
    frame_buffer_manager(
      uint32_t width,
      uint32_t height,
      const image_format& format,
      const uuid& backend_id = uuid());

    /// dtor
    ~frame_buffer_manager() noexcept;

    /// create
    [[nodiscard]] uid create() noexcept;
    /// create from
    [[nodiscard]] uid create_from(uid) noexcept;

    /// Get image format.
    [[nodiscard]] auto format() const noexcept -> image_format;
    /// Get width of buffers.
    [[nodiscard]] auto width() const noexcept -> uint32_t;
    /// Get heightautoers.
    [[nodiscard]] auto height() const noexcept -> uint32_t;
    /// Get byte sautouffer
    [[nodiscard]] auto byte_size() const noexcept -> uint64_t;

    /// Get proxy data
    [[nodiscard]] auto get_pool_object() const noexcept -> object_ptr<FrameBufferPool>;

  private:
    using buffer_manager::create;
    using buffer_manager::get_pool_object;

  private:
    image_format m_format;
    uint32_t m_width;
    uint32_t m_height;

  private:
    // proxy data for backend agnostic frame buffer management.
    object_ptr<FrameBufferPool> m_pool;
  };
}