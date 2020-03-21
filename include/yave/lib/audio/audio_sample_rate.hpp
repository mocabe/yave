//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <string>

namespace yave {

  /// Audio sample rates
  enum class audio_sample_rate : uint8_t
  {
    unknown   = 0U,  ///< Unknown
    e8000     = 1U,  ///< 8000  Hz
    e11025    = 2U,  ///< 11025 Hz
    e12000    = 3U,  ///< 12000 Hz
    e16000    = 4U,  ///< 16000 Hz
    e22050    = 5U,  ///< 22050 Hz
    e32000    = 6U,  ///< 32000 Hz
    e37800    = 7U,  ///< 37800 Hz
    e44056    = 8U,  ///< 44056 Hz
    e44100    = 9U,  ///< 44100 Hz
    e47250    = 10U, ///< 47250 Hz
    e48000    = 11U, ///< 48000 Hz
    e50000    = 12U, ///< 50000 Hz
    e50400    = 13U, ///< 50400 Hz
    e64000    = 14U, ///< 64000 Hz
    e88200    = 15U, ///< 88200 Hz
    e96000    = 16U, ///< 96000 Hz
    e176400   = 17U, ///< 176400 Hz
    e192000   = 18U, ///< 192000 Hz
    e352800   = 19U, ///< 352800 Hz
    e2822400  = 20U, ///< 2822400 Hz
    e5644800  = 21U, ///< 5644800 Hz
    e11289600 = 22U, ///< 11289600 Hz
    e22579200 = 23U, ///< 22579200 Hz
  };

  [[nodiscard]] constexpr auto to_cstr(const audio_sample_rate& rate)
  {
    // clang-format off
    switch (rate) {
      case audio_sample_rate::unknown:   return "(unknown audio sample rate)";
      case audio_sample_rate::e8000:     return "8000  Hz";
      case audio_sample_rate::e11025:    return "11025 Hz";
      case audio_sample_rate::e12000:    return "12000 Hz";
      case audio_sample_rate::e16000:    return "16000 Hz";
      case audio_sample_rate::e22050:    return "22050 Hz";
      case audio_sample_rate::e32000:    return "32000 Hz";
      case audio_sample_rate::e37800:    return "37800 Hz";
      case audio_sample_rate::e44056:    return "44056 Hz";
      case audio_sample_rate::e44100:    return "44100 Hz";
      case audio_sample_rate::e47250:    return "47250 Hz";
      case audio_sample_rate::e48000:    return "48000 Hz";
      case audio_sample_rate::e50000:    return "50000 Hz";
      case audio_sample_rate::e50400:    return "50400 Hz";
      case audio_sample_rate::e64000:    return "64000 Hz";
      case audio_sample_rate::e88200:    return "88200 Hz";
      case audio_sample_rate::e96000:    return "96000 Hz";
      case audio_sample_rate::e176400:   return "176400 Hz";
      case audio_sample_rate::e192000:   return "192000 Hz";
      case audio_sample_rate::e352800:   return "352800 Hz";
      case audio_sample_rate::e2822400:  return "2822400 Hz";
      case audio_sample_rate::e5644800:  return "5644800 Hz";
      case audio_sample_rate::e11289600: return "11289600 Hz";
      case audio_sample_rate::e22579200: return "22579200 Hz";
      default:                           unreachable();
    }
    // clang-format on
  }

  [[nodiscard]] inline auto to_string(const audio_sample_rate& rate)
  {
    return std::string(to_cstr(rate));
  }

} // namespace yave