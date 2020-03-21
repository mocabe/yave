//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <string>

namespace yave {

  /// Audio channel layout
  enum class audio_channel_layout : uint8_t
  {
    unknown = 0U, ///< Unknown
    mono    = 1U, ///< mono 1ch
    stereo  = 2U, ///< stereo 2ch
    // TODO: Add surround configurations
  };

  [[nodiscard]] constexpr auto get_channel_size(
    const audio_channel_layout& layout)
  {
    // clang-format off
    switch (layout) {
      case audio_channel_layout::mono:    return 1;
      case audio_channel_layout::stereo:  return 2;
      default:                            unreachable();
    }
    // clang-format on
  }

  [[nodiscard]] constexpr auto to_cstr(const audio_channel_layout& layout)
  {
    // clang-format off
    switch (layout) {
      case audio_channel_layout::unknown: return "(unknown channel layout)";
      case audio_channel_layout::mono:    return "mono";
      case audio_channel_layout::stereo:  return "stereo";
      default:                            unreachable();
    }
    // clang-format on
  }

  [[nodiscard]] inline auto to_string(const audio_channel_layout& layout)
  {
    return std::string(to_cstr(layout));
  }
}