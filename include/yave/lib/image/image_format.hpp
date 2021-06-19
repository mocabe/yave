//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>

#include <yave/lib/image/image_color_type.hpp>
#include <yave/lib/image/image_data_type.hpp>

namespace yave {

  struct image_format
  {
    /// pixel format
    image_color_type color_type;
    /// sample format
    image_data_type data_type;
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
  inline constexpr image_format image_format::unknown = {image_color_type::unknown, image_data_type::unknown,          0};
  inline constexpr image_format image_format::rgba8   = {image_color_type::rgba,    image_data_type::unsigned_integer, 1};
  inline constexpr image_format image_format::rgba16  = {image_color_type::rgba,    image_data_type::unsigned_integer, 2};
  inline constexpr image_format image_format::rgba32f = {image_color_type::rgba,    image_data_type::floating_point,   4};
  inline constexpr image_format image_format::rgb8    = {image_color_type::rgb,     image_data_type::unsigned_integer, 1};
  inline constexpr image_format image_format::rgb16   = {image_color_type::rgb,     image_data_type::unsigned_integer, 2};
  // clang-format on

  /// operator==
  [[nodiscard]] constexpr bool operator==(
    const image_format& lhs,
    const image_format& rhs)
  {
    return lhs.color_type == rhs.color_type && lhs.data_type == rhs.data_type
           && lhs.byte_per_channel == rhs.byte_per_channel;
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
    return get_channel_size(fmt.color_type);
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
    return fmt.byte_per_channel * get_channel_size(fmt.color_type);
  }

  /// Get string name of image format
  [[nodiscard]] std::string to_string(const image_format& fmt);

} // namespace yave
