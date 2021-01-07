//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/ops/ops.hpp>
#include <yave/module/std/config.hpp>
#include <yave/module/std/math/tag.hpp>

namespace yave {

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Ops::Add, modules::_std::math::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Ops::Sub, modules::_std::math::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Ops::Mul, modules::_std::math::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Ops::Div, modules::_std::math::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Ops::Eq, modules::_std::math::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Ops::Neq, modules::_std::math::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Ops::Less, modules::_std::math::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Ops::LessEq, modules::_std::math::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Ops::Greater, modules::_std::math::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Ops::GreaterEq, modules::_std::math::tag);
}