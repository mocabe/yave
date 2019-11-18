//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/primitive/primitive.hpp>

namespace yave {

  namespace node {
    /// If
    struct If;
  } // namespace node

  template <>
  struct node_info_traits<node::If>
  {
    static auto get_node_info() -> node_info
    {
      return {"If", {"cond", "then", "else"}, {"out"}};
    }

    static auto get_node_type() -> object_ptr<const Type>
    {
      class X;
      return object_type<node_closure<Bool, X, X>>();
    }
  };

  std::vector<node_info> get_control_flow_node_info_list()
  {
    return {get_node_info<node::If>()};
  }
}