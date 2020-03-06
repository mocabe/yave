//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/obj/color/color.hpp>

namespace yave {

  namespace node {
    // Color ctor node
    class Color;
  } // namespace node

  template <>
  struct node_declaration_traits<node::Color>
  {
    static auto get_node_declaration()
    {
      return node_declaration(
        "Color", "std::color", "Color", {"R", "G", "B", "A"}, {"color"});
    }
  };
} // namespace yave