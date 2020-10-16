//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/primitive/primitive.hpp>

namespace yave {

  namespace node::String {
    using String = PrimitiveCtor<yave::String>;
  }

  YAVE_DECL_PRIM_NODE(String::String, String.String);

} // namespace yave
