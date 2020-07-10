//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node {
    /// map
    struct ListMap;
    /// repeat
    struct ListRepeat;
    /// enumerate
    struct ListEnumerate;
    /// foldl
    struct ListFold;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::ListMap);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::ListRepeat);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::ListEnumerate);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::ListFold);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::ListMap, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::ListRepeat, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::ListEnumerate, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::ListFold, modules::_std::tag);
}