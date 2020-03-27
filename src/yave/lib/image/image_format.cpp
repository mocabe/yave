//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/lib/image/image_format.hpp>

#include <fmt/format.h>

namespace yave {

  std::string to_string(const image_format& fmt)
  {
    // ex. RGBA32FloatingPoint
    return fmt::format(
      "{}{} {}",
      to_cstr(fmt.color_type),
      std::to_string(fmt.byte_per_channel * 8),
      to_cstr(fmt.data_type));
  }
} // namespace yave