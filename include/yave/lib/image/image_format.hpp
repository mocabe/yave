//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#include <yave/lib/image/sample_format.hpp>
#include <yave/lib/image/pixel_format.hpp>

namespace yave {

  struct image_format {
    /// pixel format
    yave::pixel_format pixel_format;
    /// sample format
    yave::sample_format sample_format;
    /// byte per channel
    uint16_t byte_per_channel;

  public: /* constants */
    static const image_format unknown;
    static const image_format rgba8;
    static const image_format rgba16;
    static const image_format rgba32f;
    static const image_format rgb8;
    static const image_format rgb16;
  };

  static_assert(sizeof(image_format) == sizeof(uint32_t));

  // clang-format off
  inline constexpr image_format image_format::unknown = {pixel_format::unknown, sample_format::unknown,         0};
  inline constexpr image_format image_format::rgba8   = {pixel_format::rgba,    sample_format::unsigned_integer, 1};
  inline constexpr image_format image_format::rgba16  = {pixel_format::rgba,    sample_format::unsigned_integer, 2};
  inline constexpr image_format image_format::rgba32f = {pixel_format::rgba,    sample_format::floating_point,   4};
  inline constexpr image_format image_format::rgb8    = {pixel_format::rgb,     sample_format::unsigned_integer, 1};
  inline constexpr image_format image_format::rgb16   = {pixel_format::rgb,     sample_format::unsigned_integer, 2};
  // clang-format on

  /// operator==
  [[nodiscard]] constexpr bool operator==(
    const image_format& lhs,
    const image_format& rhs)
  {
    return lhs.pixel_format == rhs.pixel_format &&
           lhs.sample_format == rhs.sample_format &&
           lhs.byte_per_channel == rhs.byte_per_channel;
  }

  /// operator!=
  [[nodiscard]] constexpr bool operator!=(
    const image_format& lhs,
    const image_format& rhs)
  {
    return !(lhs == rhs);
  }

  /// Get channel size
  [[nodiscard]] constexpr uint32_t channel_size(
    const image_format& fmt) noexcept
  {
    return get_channel_size(fmt.pixel_format);
  }

  /// Get byte per channel
  [[nodiscard]] constexpr uint32_t byte_per_channel(
    const image_format& fmt) noexcept
  {
    return fmt.byte_per_channel;
  }

  /// Get byte per pixel
  [[nodiscard]] constexpr uint32_t byte_per_pixel(
    const image_format& fmt) noexcept
  {
    return fmt.byte_per_channel * get_channel_size(fmt.pixel_format);
  }

  /// Get string name of image format
  [[nodiscard]] std::string to_string(const image_format& fmt);

} // namespace yave
