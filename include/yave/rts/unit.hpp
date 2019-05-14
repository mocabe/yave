//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>

namespace yave {

  /// UnitValue
  struct UnitValue
  {
  };

  /// Unit
  using Unit = Box<UnitValue>;

} // namespace yave

// Unit
YAVE_DECL_TYPE(yave::Unit, "916111ba-6012-4b48-af82-df329c4d7b7b");