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
    static const image_format Unknown;
    static const image_format RGBA8UI;
    static const image_format RGBA16UI;
    static const image_format RGBA32F;
  };

  static_assert(sizeof(image_format) == sizeof(uint32_t));

  // clang-format off
  constexpr image_format image_format::Unknown  = {pixel_format::Unknown, sample_format::Unknown,         0};
  constexpr image_format image_format::RGBA8UI  = {pixel_format::RGBA,    sample_format::UnsignedInteger, 1};
  constexpr image_format image_format::RGBA16UI = {pixel_format::RGBA,    sample_format::UnsignedInteger, 2};
  constexpr image_format image_format::RGBA32F  = {pixel_format::RGBA,    sample_format::FloatingPoint,   4};
  // clang-format on

  /// operator==
  constexpr bool operator==(const image_format& lhs, const image_format& rhs)
  {
    return lhs.pixel_format == rhs.pixel_format &&
           lhs.sample_format == rhs.sample_format &&
           lhs.byte_per_channel == rhs.byte_per_channel;
  }

  /// operator!=
  constexpr bool operator!=(const image_format& lhs, const image_format& rhs)
  {
    return !(lhs == rhs);
  }

  /// Get channel size
  constexpr uint32_t channel_size(const image_format& fmt) noexcept
  {
    return get_channel_size(fmt.pixel_format);
  }

  /// Get byte per channel
  constexpr uint32_t byte_per_channel(const image_format& fmt) noexcept
  {
    return fmt.byte_per_channel;
  }

  /// Get byte per pixel
  constexpr uint32_t byte_per_pixel(const image_format& fmt) noexcept
  {
    return fmt.byte_per_channel * get_channel_size(fmt.pixel_format);
  }

  /// Get string name of image format
  std::string to_string(const image_format& fmt);

} // namespace yave
