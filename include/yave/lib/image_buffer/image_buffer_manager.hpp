//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/lib/image/image_format.hpp>
#include <yave/obj/image_buffer/image_buffer_pool.hpp>
#include <yave/support/id.hpp>
#include <yave/support/uuid.hpp>

namespace yave {

  /// Image buffer manager
  class image_buffer_manager
  {
  public:
    image_buffer_manager(const uuid& backend_id);
    ~image_buffer_manager() noexcept;

    image_buffer_manager(const image_buffer_manager&) = delete;
    image_buffer_manager& operator=(const image_buffer_manager&) = delete;

    image_buffer_manager(image_buffer_manager&&) noexcept;
    image_buffer_manager& operator=(image_buffer_manager&&) noexcept;

  public:
    /// create
    [[nodiscard]] uid create(
      uint32_t width,
      uint32_t height,
      image_format format) noexcept;

    /// create from
    [[nodiscard]] uid create_from(uid) noexcept;

    /// ref
    void ref(uid id) noexcept;
    /// unref
    void unref(uid id) noexcept;

    /// Get use count
    [[nodiscard]] auto use_count(uid id) const noexcept -> uint64_t;
    /// Get data pointer
    [[nodiscard]] auto data(uid id) noexcept -> std::byte*;

    /// Get width of buffers.
    [[nodiscard]] auto width(uid id) const noexcept -> uint32_t;
    /// Get heightautoers.
    [[nodiscard]] auto height(uid id) const noexcept -> uint32_t;
    /// Get image format.
    [[nodiscard]] auto format(uid id) const noexcept -> image_format;

    /// Get proxy data
    [[nodiscard]] auto get_pool_object() const noexcept
      -> object_ptr<const ImageBufferPool>;

  private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
  };
} // namespace yave