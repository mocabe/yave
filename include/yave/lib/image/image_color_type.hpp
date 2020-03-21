//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

namespace yave {

  /// image pixel format
  enum class image_color_type : uint8_t
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
    const image_color_type& fmt) noexcept
  {
    // clang-format off
    switch (fmt) {
      case image_color_type::unknown: return 0;
      case image_color_type::y:       return 1;
      case image_color_type::ya:      return 2;
      case image_color_type::rgb:     return 3;
      case image_color_type::ycbcr:   return 3;
      case image_color_type::cielab:  return 3;
      case image_color_type::icclab:  return 3;
      case image_color_type::rgba:    return 4;
      case image_color_type::cmyk:    return 4;
      case image_color_type::ycck:    return 4;
      default:                    unreachable();
    }
    // clang-format on
  }

  /// Check if the format has alpha channel
  [[nodiscard]] constexpr bool has_alpha_channel(
    const image_color_type& fmt) noexcept
  {
    // clang-format off
    switch (fmt) {
      case image_color_type::unknown: return false;
      case image_color_type::y:       return false;
      case image_color_type::ya:      return true;
      case image_color_type::rgb:     return false;
      case image_color_type::ycbcr:   return false;
      case image_color_type::cielab:  return false;
      case image_color_type::icclab:  return false;
      case image_color_type::rgba:    return true;
      case image_color_type::cmyk:    return false;
      case image_color_type::ycck:    return false;
      default:                    unreachable();
    }
    // clang-format on
  }

  /// Get string representation of pixel format
  [[nodiscard]] constexpr const char* to_cstr(const image_color_type& fmt)
  {
    // clang-format off
    switch (fmt) {
      case image_color_type::unknown: return "(unknown pixel color format)";
      case image_color_type::y:       return "Y";
      case image_color_type::ya:      return "YA";
      case image_color_type::rgb:     return "RGB";
      case image_color_type::ycbcr:   return "YCbCr";
      case image_color_type::cielab:  return "CIELab";
      case image_color_type::icclab:  return "ICCLab";
      case image_color_type::rgba:    return "RGBA";
      case image_color_type::cmyk:    return "CMYK";
      case image_color_type::ycck:    return "YCCK";
      default:                    unreachable();
    }
    // clang-format on
  }

  /// Convert pixel_format to string
  [[nodiscard]] inline std::string to_string(const image_color_type& format)
  {
    return {to_cstr(format)};
  }
} // namespace yave