//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/data/shape/shape_cmd.hpp>

namespace yave {
  /// shape command object
  using ShapeCmd = Box<data::shape_cmd>;
} // namespace yave

YAVE_DECL_TYPE(yave::ShapeCmd, "f53939fa-d233-4844-8e26-4130bdadfd2f");