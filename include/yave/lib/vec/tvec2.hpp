//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <glm/detail/type_vec2.hpp>

namespace yave {
  // delegate to glm for now
  template <class T>
  using tvec2 = glm::tvec2<T>;
}