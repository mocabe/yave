//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node::List {
    /// Nil
    struct Nil;
    /// Cons
    struct Cons;
    /// Decompose
    struct Decompose;
  } // namespace node::List

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::List::Nil);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::List::Cons);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::List::Decompose);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::List::Nil, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::List::Cons, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::List::Decompose, modules::_std::tag);
} // namespace yave