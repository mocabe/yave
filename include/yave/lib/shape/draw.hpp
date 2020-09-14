//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/shape/shape.hpp>
#include <yave/lib/image/image.hpp>
#include <yave/data/color/color.hpp>

namespace yave {

  /// Draw shape onto RGBA32 (BGRA8888/le) image
  [[nodiscard]] auto draw_shape_bgra8(
    const shape& s,
    uint32_t width,
    uint32_t height) -> image;
}