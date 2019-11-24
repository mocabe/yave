//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/image/image.hpp>

namespace yave {

  namespace node {
    /// Load image
    struct LoadImage;
  } // namespace node

  template <>
  struct node_declaration_traits<node::LoadImage>
  {
    static auto get_node_declaration() -> node_declaration
    {
      class X;
      return node_declaration(
        "LoadImage",
        {"path"},
        {"image"},
        node_type::normal,
        object_type<node_closure<X, Image>>()
      );
    }
  };
} // namespace yave