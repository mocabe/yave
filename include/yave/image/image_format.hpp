//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#pragma once

namespace yave {

  /// Image format supported for data transfar.
  enum class image_format : uint32_t
  {
    Unknown = 0,
    /// 8bit [0~255]
    RGBA8UI = 1U,
    /// 16bit [0~65535]
    RGBA16UI = 2U,
    /// 32bit [0.0 ~ 1.0]
    RGBA32F = 3U,
  };

  /// Get string name of image format
  constexpr const char* image_format_string(const image_format& fmt) noexcept
  {
    switch (fmt) {
      case image_format::RGBA8UI:
        return "RGBA8UI";
      case image_format::RGBA16UI:
        return "RGBA16UI";
      case image_format::RGBA32F:
        return "RGBA32F";
      default:
        return "(unrecognized image format)";
    }
  }

  /// Get channel size
  constexpr uint32_t channel_size(const image_format& fmt) noexcept
  {
    switch (fmt) {
      case image_format::RGBA8UI:
      case image_format::RGBA16UI:
      case image_format::RGBA32F:
        return 4;
      default:
        return 0;
    }
  }

  /// Get byte per channel
  constexpr uint32_t byte_per_channel(const image_format& fmt) noexcept
  {
    switch (fmt) {
      case image_format::RGBA8UI:
        return 1;
      case image_format::RGBA16UI:
        return 2;
      case image_format::RGBA32F:
        return 4;
      default:
        return 0;
    }
  }

  /// Get byte per pixel
  constexpr uint32_t byte_per_pixel(const image_format& fmt) noexcept
  {
    switch (fmt) {
      case image_format::RGBA8UI:
        return 4;
      case image_format::RGBA16UI:
        return 8;
      case image_format::RGBA32F:
        return 16;
      default:
        return 0;
    }
  }

} // namespace yave
