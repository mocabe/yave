//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/obj/rect/rect.hpp>

namespace yave {

  namespace node {
    class Rect2;
    class Rect3;
  } // namespace node

  template <>
  struct node_declaration_traits<node::Rect2>
  {
    static auto get_node_declaration()
    {
      return node_declaration(
        "Rect2", "std::geometry", "2D rectangle", {"off", "ext"}, {"value"});
    }
  };

  template <>
  struct node_declaration_traits<node::Rect3>
  {
    static auto get_node_declaration()
    {
      return node_declaration(
        "Rect3", "std::geometry", "3D rectangle", {"off", "ext"}, {"value"});
    }
  };
}