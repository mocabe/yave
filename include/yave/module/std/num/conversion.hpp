//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node {

    /// Convert value to Float
    class ToFloat;
    /// Convert value to Int
    class ToInt;

  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::ToFloat);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::ToInt);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::ToFloat, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::ToInt, modules::_std::tag);
}