//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/data/shape/shape_op.hpp>

namespace yave {
  /// shape op
  using ShapeOp = Box<data::shape_op>;
} // namespace yave

YAVE_DECL_TYPE(yave::ShapeOp, "7709eab7-1cc4-4ddf-a179-0340df886665");