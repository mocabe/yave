//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node {
    // Color ctor node
    class Color;
    // Color ctor from floats
    class ColorFloat;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Color);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::ColorFloat);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Color, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::ColorFloat, modules::_std::tag);

} // namespace yave