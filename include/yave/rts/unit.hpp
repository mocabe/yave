//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>

namespace yave {

  /// UnitValue
  struct unit_value
  {
  };

  /// Unit
  using Unit = Box<unit_value>;

} // namespace yave

// Unit
YAVE_DECL_TYPE(yave::Unit, "916111ba-6012-4b48-af82-df329c4d7b7b");