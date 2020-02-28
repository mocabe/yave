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
    unknown          = 0U, ///< Unknown sample format
    unsigned_integer = 1U, ///< UInt
    signed_integer   = 2U, ///< SInt
    floating_point   = 3U, ///< Float
  };

  /// Get string representation of sample format
  [[nodiscard]] constexpr const char* get_sample_format_cstr(
    const sample_format& fmt)
  {
    // clang-format off
    switch (fmt) {
      case sample_format::unknown:            return "(unknown pixel format)";
      case sample_format::unsigned_integer:    return "UnsignedInteger";
      case sample_format::signed_integer:      return "SignedInteger";
      case sample_format::floating_point:      return "FloatingPoint";
      default:                                unreachable();
    }
    // clang-format on
  }

  /// Convert sample_format to string
  [[nodiscard]] inline std::string to_string(const sample_format& format)
  {
    return {get_sample_format_cstr(format)};
  }

} // namespace yave