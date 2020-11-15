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
    /// Head
    struct Head;
    /// Tail
    struct Tail;
    /// At
    struct At;
    /// List macro
    struct List;
  } // namespace node::List

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::List::Nil);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::List::Cons);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::List::Head);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::List::Tail);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::List::At);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::List::List);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::List::Nil, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::List::Cons, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::List::Head, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::List::Tail, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::List::At, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::List::List, modules::_std::tag);
} // namespace yave