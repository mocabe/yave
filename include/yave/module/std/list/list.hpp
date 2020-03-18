//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace node {
    /// Nil
    struct ListNil;
    /// Cons
    struct ListCons;
    /// Decompose
    struct ListDecompose;
  } // namespace node

  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::ListNil);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::ListCons);
  YAVE_DECL_DEFAULT_NODE_DECLARATION(node::ListDecompose);

  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::ListNil, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::ListCons, modules::_std::tag);
  YAVE_DECL_DEFAULT_NODE_DEFINITION(node::ListDecompose, modules::_std::tag);
}