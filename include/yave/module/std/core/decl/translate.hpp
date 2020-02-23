//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/core/decl/primitive.hpp> 

namespace yave {

  namespace node {
    /// Translate
    struct Translate;
  } // namespace node

  template <>
  struct node_declaration_traits<node::Translate>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "Translate",
        "Move object",
        "std::core::geometry",
        {"out"},
        {"target", "x", "y", "z"},
        {{1, make_data_type_holder<Float>()},
         {2, make_data_type_holder<Float>()},
         {3, make_data_type_holder<Float>()}});
    }
  };
} // namespace yave
