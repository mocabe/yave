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
  };

  [[nodiscard]] auto to_string(const audio_format& fmt) -> std::string;

} // namespace yave