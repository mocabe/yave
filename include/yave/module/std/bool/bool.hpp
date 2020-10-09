//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/primitive/primitive.hpp>

namespace yave {

  namespace node::Bool {
    using Bool = PrimitiveCtor<yave::Bool>;
  }

  YAVE_DECL_PRIM_NODE(Bool::Bool, Std.Bool.Bool);

}