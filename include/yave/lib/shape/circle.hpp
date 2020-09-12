//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/shape/shape.hpp>

namespace yave {

  /// Create path of circle.
  /// \param center center of circle
  /// \param radius radius of circle
  [[nodiscard]] auto create_circle_shape(const glm::fvec2& center, float radius)
    -> shape;

} // namespace yave