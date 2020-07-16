//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>
#include <yave/module/std/primitive/data_holder.hpp>
#include <yave/obj/color/color.hpp>

namespace yave {

  namespace node {
    // Color ctor node
    class Color;
  } // namespace node

  // data property
  using ColorDataProperty = ValueDataTypeProperty<Color>;
  YAVE_DECL_DATA_TYPE_PROPERTY(Color, Color, ColorDataProperty);

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Color);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Color, modules::_std::tag);

} // namespace yave

YAVE_DECL_TYPE(yave::ColorDataProperty, "107e7e11-1f1e-436d-85f0-42e3c44edb00");