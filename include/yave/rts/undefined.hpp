//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>

namespace yave {

  /// Undefined
  ///
  /// Null objects are treated as instances of Undefined.
  /// Undefined should not have definition. get_term() has special case for it
  /// to return `tm_value<Undefined>` as a term of Undefined.
  struct Undefined;

} // namespace yave

YAVE_DECL_TYPE(yave::Undefined, "0d8c9e31-4a16-4df3-ba1d-8a11fd0f1074");