//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/data/lib/image_format.hpp>

#include <sstream>

namespace yave {

  std::string to_string(const image_format& fmt)
  {
    std::stringstream ss;

    // ex. RGBA32FloatingPoint
    ss << get_pixel_format_string(fmt.pixel_format)
       << std::to_string(fmt.byte_per_channel * 8)
       << get_sample_format_string(fmt.sample_format);

    return ss.str();
  }
}