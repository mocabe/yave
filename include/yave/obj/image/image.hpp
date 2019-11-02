//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/lib/image/image.hpp>

namespace yave {

  /// Image object
  using Image = Box<image>;

} // namespace yave

YAVE_DECL_TYPE(yave::Image, "98a416df-5373-4142-80af-6b064fe2b664");