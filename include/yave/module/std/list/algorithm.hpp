//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node::List::Algo {
    /// map
    struct Map;
    /// repeat
    struct Repeat;
    /// enumerate
    struct Enumerate;
    /// foldl
    struct Fold;
  } // namespace node::List::Algo

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::List::Algo::Map);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::List::Algo::Repeat);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::List::Algo::Enumerate);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::List::Algo::Fold);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::List::Algo::Map, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::List::Algo::Repeat, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::List::Algo::Enumerate, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::List::Algo::Fold, modules::_std::tag);
}