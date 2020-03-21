//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <string>

namespace yave {

  /// Audio sample data size
  enum class audio_data_size : uint8_t
  {
    unknown = 0U, ///< Unknown
    e8      = 1U, ///< 8bit
    e16     = 2U, ///< 16bit
    e24     = 3U, ///< 24bit
    e32     = 4U, ///< 32bit
  };

  [[nodiscard]] constexpr auto to_cstr(const audio_data_size& size)
  {
    // clang-format off
    switch (size) {
      case audio_data_size::unknown: return "(unknown audio sample size)";
      case audio_data_size::e8:      return "8bit";
      case audio_data_size::e16:     return "16bit";
      case audio_data_size::e24:     return "24bit";
      case audio_data_size::e32:     return "32bit";
      default:                       unreachable();
    }
    // clang-format on
  }

  [[nodiscard]] auto to_string(const audio_data_size& size)
  {
    return std::string(to_cstr(size));
  }
} // namespace yave