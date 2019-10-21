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
    /// Head
    struct ListDecompose_Head;
    /// Tail
    struct ListDecompose_Tail;
  } // namespace node

  template <>
  struct node_info_traits<node::ListNil>
  {
    static node_info get_node_info()
    {
      // List<T>
      return node_info("ListNil", {}, {"nil"});
    }
  };

  template <>
  struct node_info_traits<node::ListCons>
  {
    static node_info get_node_info()
    {
      // T -> List<T> -> List<T>
      return node_info("ListCons", {"head", "tail"}, {"cons"});
    }
  };

  template <>
  struct node_info_traits<node::ListDecompose_Head>
  {
    static node_info get_node_info()
    {
      // List<T> -> T
      return node_info("ListDecompose", {"list"}, {"head"});
    }
  };

  template <>
  struct node_info_traits<node::ListDecompose_Tail>
  {
    static node_info get_node_info()
    {
      // List<T> -> List<T>
      return node_info("ListDecompose", {"list"}, {"tail"});
    }
  };
}