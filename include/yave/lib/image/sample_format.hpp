//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

namespace yave {

  /// image sample format
  enum class sample_format : uint8_t
  {
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

  /// Get string representation of sample format
  [[nodiscard]] constexpr const char* get_sample_format_cstr(
    const sample_format& fmt)
  {
    // clang-format off
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
    // clang-format on
  }

  /// Convert sample_format to string
  [[nodiscard]] inline std::string to_string(const sample_format& format)
  {
    return {get_sample_format_cstr(format)};
  }

} // namespace yave