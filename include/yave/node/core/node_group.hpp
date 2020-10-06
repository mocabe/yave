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
    /// Node dependency
    class NodeDependency;
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
      return node_declaration("_.NodeGroupInterface", "", {}, {});
    }
  };

  // dummy declaration
  template <>
  struct node_declaration_traits<node::NodeDependency>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration("_.NodeGroupDependency", "", {""}, {""});
    }
  };

  // dummy declaration
  template <>
  struct node_declaration_traits<node::NodeGroupInput>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration("_.NodeGroupInput", "", {}, {});
    }
  };

  // dummy declaration
  template <>
  struct node_declaration_traits<node::NodeGroupOutput>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration("_.NodeGroupOutput", "", {}, {});
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
        "_.NodeGroupIOBit",
        "",
        {""},  // will change dynamically
        {""}); // will change dynamically
    }
  };

} // namespace yave
