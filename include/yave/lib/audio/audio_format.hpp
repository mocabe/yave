//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/audio/audio_data_type.hpp>
#include <yave/lib/audio/audio_data_size.hpp>
#include <yave/lib/audio/audio_sample_rate.hpp>
#include <yave/lib/audio/audio_channel_layout.hpp>

namespace yave {

  /// Audio data format
  struct audio_format
  {
    /// Sample format
    audio_data_type data_type;
    /// Sample size
    audio_data_size data_size;
    /// Sample rate
    audio_sample_rate sample_rate;
    /// Channel layout
    audio_channel_layout channel_layout;

  public:
    static const audio_format unknown;
    // standard signed 16bit pcm audio formats
    static const audio_format pcm_44100_mono;
    static const audio_format pcm_44100_stereo;
    static const audio_format pcm_48000_stereo;
    static const audio_format pcm_96000_stereo;
    static const audio_format pcm_192000_stereo;
  };

  // clang-format off
  inline constexpr audio_format audio_format::unknown = {audio_data_type::unknown, audio_data_size::unknown, audio_sample_rate::unknown, audio_channel_layout::unknown};
  inline constexpr audio_format audio_format::pcm_44100_mono = {audio_data_type::signed_integer, audio_data_size::e16, audio_sample_rate::e44100, audio_channel_layout::mono};
  inline constexpr audio_format audio_format::pcm_44100_stereo = {audio_data_type::signed_integer, audio_data_size::e16, audio_sample_rate::e44100, audio_channel_layout::stereo};
  inline constexpr audio_format audio_format::pcm_48000_stereo = {audio_data_type::signed_integer, audio_data_size::e16, audio_sample_rate::e48000, audio_channel_layout::stereo};
  inline constexpr audio_format audio_format::pcm_96000_stereo = {audio_data_type::signed_integer, audio_data_size::e16, audio_sample_rate::e96000, audio_channel_layout::stereo};
  inline constexpr audio_format audio_format::pcm_96000_stereo = {audio_data_type::signed_integer, audio_data_size::e16, audio_sample_rate::e192000, audio_channel_layout::stereo};
  // clang-format on

  [[nodiscard]] constexpr bool operator==(
    const audio_format& lhs,
    const audio_format& rhs)
  {
    return lhs.data_type == rhs.data_type && lhs.data_size == rhs.data_size
           && lhs.sample_rate == rhs.sample_rate
           && lhs.channel_layout == rhs.channel_layout;
  }

  [[nodiscard]] constexpr bool operator!=(
    const audio_format& lhs,
    const audio_format& rhs)
  {
    return !(lhs == rhs);
  }

  [[nodiscard]] auto to_string(const audio_format& fmt) -> std::string;

} // namespace yave