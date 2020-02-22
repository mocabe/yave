//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <glm/mat4x4.hpp>

namespace yave {
  /// delegate to glm
  template <class T>
  using tmat4 = glm::tmat4x4<T>;
}