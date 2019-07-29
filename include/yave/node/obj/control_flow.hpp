//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>

namespace yave {

  /// If
  struct IfNode;

  template <>
  struct node_info_traits<IfNode>
  {
    static node_info get_node_info()
    {
      return {"If", {"cond", "then", "else"}, {"out"}};
    }
  };
}