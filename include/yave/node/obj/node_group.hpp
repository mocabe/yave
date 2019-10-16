//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>

namespace yave {

  namespace node {
    /// Interface node which represents node group
    class NodeGroupInterface;
    /// Interface node which represents node input
    class NodeGroupInput;
    /// Interface node which represents node output
    class NodeGroupOutput;
    /// Node group I/O socket node
    class NodeGroupIOBit;
  } // namespace node

  template <>
  struct node_info_traits<node::NodeGroupInterface>
  {
    static node_info get_node_info()
    {
      return node_info {"NodeGroupInterface", {}, {}, node_type::interface};
    }
  };

  template <>
  struct node_info_traits<node::NodeGroupInput>
  {
    static node_info get_node_info()
    {
      return node_info {"NodeGroupInput", {}, {}, node_type::interface};
    }
  };

  template <>
  struct node_info_traits<node::NodeGroupOutput>
  {
    static node_info get_node_info()
    {
      return node_info {"NodeGroupOutput", {}, {}, node_type::interface};
    }
  };

  template <>
  struct node_info_traits<node::NodeGroupIOBit>
  {
    static node_info get_node_info()
    {
      // socket names will change dynamically
      return node_info {"NodeGroupIOBit", {""}, {""}};
    }
  };

} // namespace yave
