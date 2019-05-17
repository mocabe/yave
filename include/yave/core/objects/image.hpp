//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/image/image.hpp>
#include <yave/rts/box.hpp>

namespace yave {

  /// Image object
  using Image = Box<image>;

} // namespace yave

YAVE_DECL_TYPE(yave::Image, "98a416df-5373-4142-80af-6b064fe2b664");