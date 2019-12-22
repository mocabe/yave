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
    Unknown = 0U, ///< unknown pixel format.
    Y       = 1U, ///< 1-channel format: Y
    YA      = 2U, ///< 2-channel format: YA
    RGB     = 3U, ///< 3-channel format: RGB
    YCbCr   = 4U, ///< 3-channel format: YCbCr
    CIELab  = 5U, ///< 3-channel format: CIELab
    ICCLab  = 6U, ///< 3-channel format: ICCLab
    RGBA    = 7U, ///< 4-channel format: RGBA
    CMYK    = 8U, ///< 4-channel format: CMYK
    YCCK    = 9U, ///< 4-channel format: YCCK
  };

  /// Get number of channels
  [[nodiscard]] constexpr uint8_t get_channel_size(
    const pixel_format& fmt) noexcept
  {
    // clang-format off
    switch (fmt) {
      case pixel_format::Unknown: return 0;
      case pixel_format::Y:       return 1;
      case pixel_format::YA:      return 2;
      case pixel_format::RGB:     return 3;
      case pixel_format::YCbCr:   return 3;
      case pixel_format::CIELab:  return 3;
      case pixel_format::ICCLab:  return 3;
      case pixel_format::RGBA:    return 4;
      case pixel_format::CMYK:    return 4;
      case pixel_format::YCCK:    return 4;
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
      case pixel_format::Unknown: return false;
      case pixel_format::Y:       return false;
      case pixel_format::YA:      return false;
      case pixel_format::RGB:     return false;
      case pixel_format::YCbCr:   return false;
      case pixel_format::CIELab:  return false;
      case pixel_format::ICCLab:  return false;
      case pixel_format::RGBA:    return true;
      case pixel_format::CMYK:    return false;
      case pixel_format::YCCK:    return false;
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
      case pixel_format::Unknown: return "(unknown pixel format)";
      case pixel_format::Y:       return "R";
      case pixel_format::YA:      return "RG";
      case pixel_format::RGB:     return "RGB";
      case pixel_format::YCbCr:   return "YCbCr";
      case pixel_format::CIELab:  return "CIELab";
      case pixel_format::ICCLab:  return "ICCLab";
      case pixel_format::RGBA:    return "RGBA";
      case pixel_format::CMYK:    return "CMYK";
      case pixel_format::YCCK:    return "YCCK";
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