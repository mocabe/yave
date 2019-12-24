//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/node/core/function.hpp>

namespace yave {

  namespace node {
    /// Nil
    struct ListNil;
    /// Cons
    struct ListCons;
    /// Head
    struct ListHead;
    /// Tail
    struct ListTail;
  } // namespace node

  template <>
  struct node_declaration_traits<node::ListNil>
  {
    static auto get_node_declaration() -> node_declaration
    {
      class X;
      return node_declaration(
        "ListNil", {}, {"nil"}, object_type<node_closure<List<X>>>());
    }
  };

  template <>
  struct node_declaration_traits<node::ListCons>
  {
    static auto get_node_declaration() -> node_declaration
    {
      class X;
      return node_declaration(
        "ListCons",
        {"head", "tail"},
        {"cons"},
        object_type<node_closure<X, List<X>, List<X>>>());
    }
  };

  template <>
  struct node_declaration_traits<node::ListHead>
  {
    static auto get_node_declaration() -> node_declaration
    {
      class X;
      return node_declaration(
        "ListDecompose",
        {"list"},
        {"head"},
        {object_type<node_closure<List<X>, X>>()});
    }
  };

  template <>
  struct node_declaration_traits<node::ListTail>
  {
    static auto get_node_declaration() -> node_declaration
    {
      class X;
      return node_declaration(
        "ListDecompose",
        {"list"},
        {"tail"},
        {object_type<node_closure<List<X>, List<X>>>()});
    }
  };
}