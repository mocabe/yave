//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

namespace yave {

  /// image pixel format
  enum class pixel_format : uint8_t
  {
    unknown = 0U, ///< unknown pixel format.
    y       = 1U, ///< 1-channel format: Y
    ya      = 2U, ///< 2-channel format: YA
    rgb     = 3U, ///< 3-channel format: RGB
    ycbcr   = 4U, ///< 3-channel format: YCbCr
    cielab  = 5U, ///< 3-channel format: CIELab
    icclab  = 6U, ///< 3-channel format: ICCLab
    rgba    = 7U, ///< 4-channel format: RGBA
    cmyk    = 8U, ///< 4-channel format: CMYK
    ycck    = 9U, ///< 4-channel format: YCCK
  };

  /// Get number of channels
  [[nodiscard]] constexpr uint8_t get_channel_size(
    const pixel_format& fmt) noexcept
  {
    // clang-format off
    switch (fmt) {
      case pixel_format::unknown: return 0;
      case pixel_format::y:       return 1;
      case pixel_format::ya:      return 2;
      case pixel_format::rgb:     return 3;
      case pixel_format::ycbcr:   return 3;
      case pixel_format::cielab:  return 3;
      case pixel_format::icclab:  return 3;
      case pixel_format::rgba:    return 4;
      case pixel_format::cmyk:    return 4;
      case pixel_format::ycck:    return 4;
      default:                    unreachable();
    }
    // clang-format on
  }

  /// Check if the format has alpha channel
  [[nodiscard]] constexpr bool has_alpha_channel(
    const pixel_format& fmt) noexcept
  {
    // clang-format off
    switch (fmt) {
      case pixel_format::unknown: return false;
      case pixel_format::y:       return false;
      case pixel_format::ya:      return true;
      case pixel_format::rgb:     return false;
      case pixel_format::ycbcr:   return false;
      case pixel_format::cielab:  return false;
      case pixel_format::icclab:  return false;
      case pixel_format::rgba:    return true;
      case pixel_format::cmyk:    return false;
      case pixel_format::ycck:    return false;
      default:                    unreachable();
    }
    // clang-format on
  }

  /// Get string representation of pixel format
  [[nodiscard]] constexpr const char* get_pixel_format_cstr(
    const pixel_format& fmt)
  {
    // clang-format off
    switch (fmt) {
      case pixel_format::unknown: return "(unknown pixel format)";
      case pixel_format::y:       return "Y";
      case pixel_format::ya:      return "YA";
      case pixel_format::rgb:     return "RGB";
      case pixel_format::ycbcr:   return "YCbCr";
      case pixel_format::cielab:  return "CIELab";
      case pixel_format::icclab:  return "ICCLab";
      case pixel_format::rgba:    return "RGBA";
      case pixel_format::cmyk:    return "CMYK";
      case pixel_format::ycck:    return "YCCK";
      default:                    unreachable();
    }
    // clang-format on
  }

  /// Convert pixel_format to string
  [[nodiscard]] inline std::string to_string(const pixel_format& format)
  {
    return {get_pixel_format_cstr(format)};
  }
} // namespace yave