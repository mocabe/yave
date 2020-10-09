//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node::List {
    /// map
    struct Map;
    /// repeat
    struct Repeat;
    /// enumerate
    struct Enumerate;
    /// foldl
    struct Fold;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::List::Map);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::List::Repeat);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::List::Enumerate);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::List::Fold);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::List::Map, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::List::Repeat, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::List::Enumerate, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::List::Fold, modules::_std::tag);
}