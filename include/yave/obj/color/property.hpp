//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/obj/node/argument_property.hpp>
#include <yave/obj/color/color.hpp>

namespace yave {

  // data property
  using ColorDataProperty = ValueArgumentProperty<Color>;
  YAVE_DECL_NODE_ARGUMENT_PROPERTY(Color, Color, ColorDataProperty);

} // namespace yave

YAVE_DECL_TYPE(yave::ColorDataProperty, "107e7e11-1f1e-436d-85f0-42e3c44edb00");