//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>

namespace yave {

  namespace node {
    /// Nil
    struct ListNil;
    /// Cons
    struct ListCons;
    /// Decompose
    struct ListDecompose;
  } // namespace node

  template <>
  struct node_declaration_traits<node::ListNil>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "ListNil",
        "Constructs empty list. Result can be passed to any type of node which "
        "takes list as input.",
        "std::core::list",
        {},
        {"[]"});
    }
  };

  template <>
  struct node_declaration_traits<node::ListCons>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "ListCons",
        "Construct new list node",
        "std::core::list",
        {"head", "tail"},
        {"cons"});
    }
  };

  template <>
  struct node_declaration_traits<node::ListDecompose>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "ListDecompose",
        "Decompose list into head and tail",
        "std::core::list",
        {"list"},
        {"head", "tail"});
    }
  };
}