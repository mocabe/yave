//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/obj/node/argument_property.hpp>
#include <yave/obj/vec/vec.hpp>

namespace yave {

  using Vec2DataProperty = ValueArgumentProperty<Vec2>;
  YAVE_DECL_NODE_ARGUMENT_PROPERTY(Vec2, Vec2, Vec2DataProperty);
} // namespace yave

YAVE_DECL_TYPE(yave::Vec2DataProperty, "0b29b7c3-3c2f-4119-867a-d739b39964f0");