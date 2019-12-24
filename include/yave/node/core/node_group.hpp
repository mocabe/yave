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

  // dummy declaration
  template <>
  struct node_declaration_traits<node::NodeGroupInterface>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "NodeGroupInterface",
        {},
        {},
        object_type<closure<Undefined, Undefined>>());
    }
  };

  // dummy declaration
  template <>
  struct node_declaration_traits<node::NodeGroupInput>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "NodeGroupInput", {}, {}, object_type<closure<Undefined, Undefined>>());
    }
  };

  // dummy declaration
  template <>
  struct node_declaration_traits<node::NodeGroupOutput>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "NodeGroupOutput",
        {},
        {},
        object_type<closure<Undefined, Undefined>>());
    }
  };

  // dummy declaration
  template <>
  struct node_declaration_traits<node::NodeGroupIOBit>
  {
    static auto get_node_declaration() -> node_declaration
    {
      class X;
      return node_declaration(
        "NodeGroupIOBit",
        {""}, // will change dynamically
        {""}, // will change dynamically
        {object_type<node_closure<X, X>>()});
    }
  };

} // namespace yave
