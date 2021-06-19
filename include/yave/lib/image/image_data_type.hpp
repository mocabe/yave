//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>

#include <string>

namespace yave {

  /// image sample format
  enum class image_data_type : uint8_t
  {
    unknown          = 0U, ///< Unknown sample format
    unsigned_integer = 1U, ///< UInt
    signed_integer   = 2U, ///< SInt
    floating_point   = 3U, ///< Float
  };

  /// Get string representation of sample format
  [[nodiscard]] constexpr const char* to_cstr(const image_data_type& fmt)
  {
    // clang-format off
    switch (fmt) {
      case image_data_type::unknown:          return "(unknown pixel data format)";
      case image_data_type::unsigned_integer: return "Unsigned Integer";
      case image_data_type::signed_integer:   return "Signed Integer";
      case image_data_type::floating_point:   return "Floating Point";
      default:                                unreachable();
    }
    // clang-format on
  }

  /// Convert image_data_type to string
  [[nodiscard]] inline std::string to_string(const image_data_type& format)
  {
    return {to_cstr(format)};
  }

} // namespace yave