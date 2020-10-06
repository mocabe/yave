//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node {
    struct Add;
    struct Sub;
    struct Mul;
    struct Div;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Add);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Sub);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Mul);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::Div);
}