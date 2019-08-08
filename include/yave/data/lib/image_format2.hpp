//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

namespace yave {

  // clang-format off

  /// image pixel format
  enum class pixel_format : uint8_t {
    Unknown = 0U, ///< unknown pixel format.
    R       = 1U, ///< 1-channel format: R
    RG      = 2U, ///< 2-channel format: RG
    RGB     = 3U, ///< 3-channel format: RGB
    YCbCr   = 4U, ///< 3-channel format: YCbCr
    CIELab  = 5U, ///< 3-channel format: CIELab
    ICCLab  = 6U, ///< 3-channel format: ICCLab
    RGBA    = 7U, ///< 4-channel format: RGBA
    CMYK    = 8U, ///< 4-channel format: CMYK
    YCCK    = 9U, ///< 4-channel format: YCCK
  };

  /// image sample format
  enum class sample_format : uint8_t {
    Unknown            = 0U, ///< Unknown sample format
    UnsignedInteger    = 1U, ///< UInt
    SignedInteger      = 2U, ///< SInt
    FloatingPoint      = 3U, ///< Float
    UnsignedNormalized = 4U, ///< UNorm
    SignedNormalized   = 5U, ///< SNorm
    UnsignedScaled     = 6U, ///< UScaled
    SignedScaled       = 7U, ///< SScaled
    SRGB               = 8U, ///< UNorm (SRGB nonlinear encoding)
  };

  /// Get number of channels
  constexpr uint8_t get_channel_size(const pixel_format& fmt) noexcept {
    switch (fmt) {
      case pixel_format::Unknown: return 0;
      case pixel_format::R:       return 1;
      case pixel_format::RG:      return 2;
      case pixel_format::RGB:     return 3;
      case pixel_format::YCbCr:   return 3;
      case pixel_format::CIELab:  return 3;
      case pixel_format::ICCLab:  return 3;
      case pixel_format::RGBA:    return 4;
      case pixel_format::CMYK:    return 4;
      case pixel_format::YCCK:    return 4;
      default:                    return 0;
    }
  }

  /// Check if the format has alpha channel
  constexpr bool has_alpha_channel(const pixel_format& fmt) noexcept {
    switch (fmt) {
      case pixel_format::Unknown: return false;
      case pixel_format::R:       return false;
      case pixel_format::RG:      return false;
      case pixel_format::RGB:     return false;
      case pixel_format::YCbCr:   return false;
      case pixel_format::CIELab:  return false;
      case pixel_format::ICCLab:  return false;
      case pixel_format::RGBA:    return true;
      case pixel_format::CMYK:    return false;
      case pixel_format::YCCK:    return false;
      default:                    return false;
    }
  }

  /// Get string representation of pixel format
  constexpr const char* get_pixel_format_string(const pixel_format& fmt) {
    switch (fmt) {
      case pixel_format::Unknown: return "(unknown pixel format)";
      case pixel_format::R:       return "R";
      case pixel_format::RG:      return "RG";
      case pixel_format::RGB:     return "RGB";
      case pixel_format::YCbCr:   return "YCbCr";
      case pixel_format::CIELab:  return "CIELab";
      case pixel_format::ICCLab:  return "ICCLab";
      case pixel_format::RGBA:    return "RGBA";
      case pixel_format::CMYK:    return "CMYK";
      case pixel_format::YCCK:    return "YCCK";
      default:                    return "(unrecognized pixel format)";
    }
  }

  /// Get string representation of sample format
  constexpr const char* get_sample_format_string(const sample_format& fmt) {
    switch (fmt) {
      case sample_format::Unknown:            return "(unknown pixel format)";
      case sample_format::UnsignedInteger:    return "UnsignedInteger";
      case sample_format::SignedInteger:      return "SignedInteger";
      case sample_format::FloatingPoint:      return "FloatingPoint";
      case sample_format::UnsignedNormalized: return "UnsignedNormalized";
      case sample_format::SignedNormalized:   return "SignedNormalized";
      case sample_format::UnsignedScaled:     return "UnsignedScaled";
      case sample_format::SignedScaled:       return "SignedScaled";
      case sample_format::SRGB:               return "SRGB";
      default:                                return "(unrecognized sample format)";
    }
  }

  // clang-format on

} // namespace yave