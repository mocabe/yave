//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/data/lib/image_format.hpp>
#include <yave/data/lib/image.hpp>

namespace yave {

  class mutable_image_view
  {
  public:
    /// Default ctor
    mutable_image_view() noexcept = default;
    /// Copy ctor
    mutable_image_view(const mutable_image_view&) noexcept = default;
    /// Move ctor
    mutable_image_view(mutable_image_view&&) noexcept = default;
    /// Destructor
    ~mutable_image_view() noexcept = default;

    /// Construct image_view with data pointer and meta info.
    mutable_image_view(
      uint8_t* data,
      uint32_t width,
      uint32_t height,
      yave::image_format image_format) noexcept;

    /// Construct image_view with null data pointer.
    mutable_image_view(
      uint32_t width,
      uint32_t height,
      yave::image_format image_format) noexcept;

    /// Construct image_view from image
    mutable_image_view(image& image) noexcept;

    /// Clear
    void clear() noexcept;

    /// Empty?
    [[nodiscard]] bool empty() const noexcept;

    /// Get data pointer.
    [[nodiscard]] uint8_t* data() noexcept;

    /// Get data pointer.
    [[nodiscard]] const uint8_t* data() const noexcept;

    /// Get image format.
    [[nodiscard]] yave::image_format image_format() const noexcept;

    /// Get width.
    [[nodiscard]] uint32_t width() const noexcept;

    /// Get height.
    [[nodiscard]] uint32_t height() const noexcept;

    /// Get stride.
    [[nodiscard]] uint32_t stride() const noexcept;

    /// Get number of channels.
    [[nodiscard]] uint32_t channels() const noexcept;

    /// Get total pixel number.
    [[nodiscard]] uint32_t pixels() const noexcept;

    /// Get bytes per channels.
    [[nodiscard]] uint32_t byte_per_channel() const noexcept;

    /// Get bytes per pixel.
    [[nodiscard]] uint32_t byte_per_pixel() const noexcept;

    /// Get total size in byte.
    [[nodiscard]] uint32_t byte_size() const noexcept;

  private:
    /// data pointer
    uint8_t* m_data = nullptr;
    /// width
    uint32_t m_width = 0;
    /// height
    uint32_t m_height = 0;
    /// image format
    yave::image_format m_format = yave::image_format::Unknown;
  };

  class const_image_view
  {
  public:
    /// Default ctor
    const_image_view() noexcept = default;
    /// Copy ctor
    const_image_view(const const_image_view&) noexcept = default;
    /// Move ctor
    const_image_view(const_image_view&&) noexcept = default;
    /// Destructor
    ~const_image_view() noexcept = default;

    /// Construct image_view with data pointer and meta info.
    const_image_view(
      const uint8_t* data,
      uint32_t width,
      uint32_t height,
      yave::image_format image_format) noexcept;

    /// Construct image_view with null data pointer.
    const_image_view(
      uint32_t width,
      uint32_t height,
      yave::image_format image_format) noexcept;

    /// Construct image_view from image
    const_image_view(const image& image) noexcept;

    /// Empty?
    [[nodiscard]] bool empty() const noexcept;

    /// Get data pointer.
    [[nodiscard]] const uint8_t* data() const noexcept;

    /// Get image format.
    [[nodiscard]] yave::image_format image_format() const noexcept;

    /// Get width.
    [[nodiscard]] uint32_t width() const noexcept;

    /// Get height.
    [[nodiscard]] uint32_t height() const noexcept;

    /// Get stride.
    [[nodiscard]] uint32_t stride() const noexcept;

    /// Get number of channels.
    [[nodiscard]] uint32_t channels() const noexcept;

    /// Get total pixel number.
    [[nodiscard]] uint32_t pixels() const noexcept;

    /// Get bytes per channels.
    [[nodiscard]] uint32_t byte_per_channel() const noexcept;

    /// Get bytes per pixel.
    [[nodiscard]] uint32_t byte_per_pixel() const noexcept;

    /// Get total size in byte.
    [[nodiscard]] uint32_t byte_size() const noexcept;

  private:
    /// data pointer
    const uint8_t* m_data = nullptr;
    /// width
    uint32_t m_width = 0;
    /// height
    uint32_t m_height = 0;
    /// image format
    yave::image_format m_format = yave::image_format::Unknown;
  };
} // namespace yave
