//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>
#include <yave/lib/image/image_format.hpp>
#include <yave/obj/frame_buffer/frame_buffer_pool.hpp>
#include <yave/obj/frame_buffer/frame_buffer.hpp>
#include <yave/lib/vulkan/texture.hpp>
#include <yave/lib/vulkan/offscreen_context.hpp>

#include <yave/support/id.hpp>
#include <yave/core/uuid.hpp>

#include <vector>
#include <mutex>

namespace yave::data {

  /// Simple frame buffer factory base on buffer_manager
  class frame_buffer_manager
  {
  public:
    frame_buffer_manager(
      uint32_t width,
      uint32_t height,
      image_format format,
      uuid backend_id,
      vulkan::offscreen_context& ctx);

    ~frame_buffer_manager() noexcept;

    frame_buffer_manager(const frame_buffer_manager&) = delete;
    frame_buffer_manager& operator=(const frame_buffer_manager&) = delete;

    frame_buffer_manager(frame_buffer_manager&&) noexcept;
    frame_buffer_manager& operator=(frame_buffer_manager&&) noexcept;

  public:
    /// create
    [[nodiscard]] auto create() noexcept -> uid;
    /// create from
    [[nodiscard]] auto create_from(uid) noexcept -> uid;
    /// destroy
    void destroy(uid) noexcept;

    /// valid id?
    [[nodiscard]] bool exists(uid id) const noexcept;
    /// Get width of buffers.
    [[nodiscard]] auto width() const noexcept -> uint32_t;
    /// Get heightautoers.
    [[nodiscard]] auto height() const noexcept -> uint32_t;
    /// Get image format.
    [[nodiscard]] auto format() const noexcept -> image_format;

    /// Get proxy data
    [[nodiscard]] auto get_pool_object() const noexcept
      -> object_ptr<const FrameBufferPool>;

    /// Get internal texture data
    [[nodiscard]] auto get_texture_data(uid id) -> vulkan::texture_data&;

  private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
  };
}