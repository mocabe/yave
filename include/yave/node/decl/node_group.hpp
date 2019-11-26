//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/node/core/function.hpp>

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
  struct node_declaration_traits<node::NodeGroupInterface>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "NodeGroupInterface", {}, {}, {}, node_type::interface);
    }
  };

  template <>
  struct node_declaration_traits<node::NodeGroupInput>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "NodeGroupInput", {}, {}, {}, node_type::interface);
    }
  };

  template <>
  struct node_declaration_traits<node::NodeGroupOutput>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "NodeGroupOutput", {}, {}, {}, node_type::interface);
    }
  };

  template <>
  struct node_declaration_traits<node::NodeGroupIOBit>
  {
    static auto get_node_declaration() -> node_declaration
    {
      class X;
      return node_declaration(
        "NodeGroupIOBit",
        {"in"},  // will change dynamically
        {"out"}, // will change dynamically
        {object_type<node_closure<X, X>>()},
        node_type::normal);
    }
  };

} // namespace yave
