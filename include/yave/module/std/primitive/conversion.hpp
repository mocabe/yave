//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>
#include <yave/module/std/primitive/data_holder.hpp>

namespace yave {

  namespace node {
    class IntToFloat;
    class FloatToInt;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::IntToFloat);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::IntToFloat, modules::_std::tag);
}