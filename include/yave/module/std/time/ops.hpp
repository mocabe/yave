//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/ops/ops.hpp>

namespace yave {

  namespace modules::_std::time {
    class tag;
  }

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Ops::Add, modules::_std::time::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Ops::Sub, modules::_std::time::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Ops::Eq, modules::_std::time::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Ops::Neq, modules::_std::time::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Ops::Less, modules::_std::time::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Ops::LessEq, modules::_std::time::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Ops::Greater, modules::_std::time::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Ops::GreaterEq, modules::_std::time::tag);
}