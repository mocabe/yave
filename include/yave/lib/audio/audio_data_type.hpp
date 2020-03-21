//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <string>

namespace yave {

  /// Audio sample data type
  enum class audio_data_type : uint8_t
  {
    unknown          = 0U, ///< Unknown format
    unsigned_integer = 1U, ///< Unsigned Int LE
    signed_integer   = 2U, ///< Signed Int LE
    floating_point   = 3U, ///< Float LE
  };

  [[nodiscard]] constexpr auto to_cstr(const audio_data_type& type)
  {
    // clang-format off
    switch (type) {
      case audio_data_type::unknown:          return "(unknown audio format)";
      case audio_data_type::unsigned_integer: return "unsigned int LE";
      case audio_data_type::signed_integer:   return "signed int LE";
      case audio_data_type::floating_point:   return "floating point LE";
      default:                                unreachable();
    }
    // clang-format on
  }

  [[nodiscard]] inline auto to_string(const audio_data_type& type)
  {
    return std::string(to_cstr(type));
  }
}