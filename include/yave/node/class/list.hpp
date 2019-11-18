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
    struct ListDecompose_Head;
    /// Tail
    struct ListDecompose_Tail;
  } // namespace node

  template <>
  struct node_info_traits<node::ListNil>
  {
    static auto get_node_info() -> node_info
    {
      // List<T>
      return node_info("ListNil", {}, {"nil"});
    }

    static auto get_node_type() -> object_ptr<const Type>
    {
      class X;
      return object_type<node_closure<List<X>>>();
    }
  };

  template <>
  struct node_info_traits<node::ListCons>
  {
    static auto get_node_info() -> node_info
    {
      return node_info("ListCons", {"head", "tail"}, {"cons"});
    }

    static auto get_node_type() -> object_ptr<const Type>
    {
      class X;
      return object_type<node_closure<X, List<X>, List<X>>>();
    }
  };

  template <>
  struct node_info_traits<node::ListDecompose_Head>
  {
    static auto get_node_info() -> node_info
    {
      return node_info("ListDecompose", {"list"}, {"head"});
    }

    static auto get_node_type() -> object_ptr<const Type>
    {
      class X;
      return object_type<node_closure<List<X>, X>>();
    }
  };

  template <>
  struct node_info_traits<node::ListDecompose_Tail>
  {
    static auto get_node_info() -> node_info
    {
      // List<T> -> List<T>
      return node_info("ListDecompose", {"list"}, {"tail"});
    }

    static auto get_node_type() -> object_ptr<const Type>
    {
      class X;
      return object_type<node_closure<List<X>, List<X>>>();
    }
  };
}