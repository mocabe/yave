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
    struct MathAdd;
    struct MathSubtract;
    struct MathMultiply;
    struct MathDivide;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::MathAdd);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::MathSubtract);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::MathMultiply);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::MathDivide);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::MathAdd, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::MathSubtract, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::MathMultiply, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::MathDivide, modules::_std::tag);
}