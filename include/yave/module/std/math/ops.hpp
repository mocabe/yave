//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/ops/ops.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace modules::_std::math {
    struct tag;
  } // namespace modules::_std::math

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Add, modules::_std::math::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Sub, modules::_std::math::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Mul, modules::_std::math::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Div, modules::_std::math::tag);
}