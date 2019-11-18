//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/node/core/function.hpp>

namespace yave {

  namespace node {
    /// Id function
    struct Identity;
  } // namespace node

  template <>
  struct node_info_traits<node::Identity>
  {
    static auto get_node_info() -> node_info
    {
      return {"Identity", {"in"}, {"out"}};
    }

    static auto get_node_type() -> object_ptr<const Type>
    {
      class X;
      return object_type<node_closure<X, X>>();
    }
  };
}