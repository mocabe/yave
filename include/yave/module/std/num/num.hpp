//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/primitive/primitive.hpp>

namespace yave {

  namespace node::Num {
    using Int   = PrimitiveCtor<yave::Int>;
    using Float = PrimitiveCtor<yave::Float>;
  } // namespace node::Num

  YAVE_DECL_PRIM_NODE(Num::Int, Std.Num.Int);
  YAVE_DECL_PRIM_NODE(Num::Float, Std.Num.Float);

} // namespace yave