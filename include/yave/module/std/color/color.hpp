//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node::color {
    // Color ctor node
    class Color;
    // Color ctor from floats
    class FromFloats;
  } // namespace node::color

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::color::Color);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::color::FromFloats);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::color::Color, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::color::FromFloats, modules::_std::tag);

} // namespace yave