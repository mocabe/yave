//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <glm/mat3x3.hpp>

namespace yave {
  /// delegate to glm
  template <class T>
  using tmat2 = glm::tmat2x2<T>;
}