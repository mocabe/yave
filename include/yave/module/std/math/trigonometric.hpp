//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/primitive/primitive.hpp>

namespace yave {

  namespace node::Math {
    struct Sin;
    struct Cos;
    struct Tan;
  } // namespace node::Math

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Math::Sin);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Math::Cos);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Math::Tan);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Math::Sin, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Math::Cos, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::Math::Tan, modules::_std::tag);

} // namespace yave