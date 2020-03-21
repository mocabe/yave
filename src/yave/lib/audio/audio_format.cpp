//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/audio/audio_format.hpp>

#include <fmt/format.h>

namespace yave {

  auto to_string(const audio_format& fmt) -> std::string
  {
    return fmt::format(
      "{}-{}({})-{} ",
      to_string(fmt.channel_layout),
      to_cstr(fmt.data_type),
      to_cstr(fmt.data_size),
      to_cstr(fmt.sample_rate));
  }
} // namespace yave
