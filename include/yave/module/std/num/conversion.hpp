//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node::Num {

    /// Convert value to Float
    class ToFloat;
    /// Convert value to Int
    class ToInt;

  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Num::ToFloat);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Num::ToInt);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Num::ToFloat, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Num::ToInt, modules::_std::tag);
}