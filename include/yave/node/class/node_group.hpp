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
        "NodeGroupInterface",
        {},
        {},
        [](managed_node_graph&, const node_handle&) -> node_handle {
          assert(false);
          return nullptr;
        });
    }
  };

  template <>
  struct node_declaration_traits<node::NodeGroupInput>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "NodeGroupInput",
        {},
        {},
        [](managed_node_graph&, const node_handle&) -> node_handle {
          assert(false);
          return nullptr;
        });
    }
  };

  template <>
  struct node_declaration_traits<node::NodeGroupOutput>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "NodeGroupOutput",
        {},
        {},
        [](managed_node_graph&, const node_handle&) -> node_handle {
          assert(false);
          return nullptr;
        });
    }
  };

  template <>
  struct node_declaration_traits<node::NodeGroupIOBit>
  {
    static auto get_node_declaration() -> node_declaration
    {
      // socket names will change dynamically
      class X;
      return node_declaration(
        "NodeGroupIOBit",
        {"0"},
        {"0"},
        node_type::normal,
        object_type<node_closure<X, X>>());
    }
  };

} // namespace yave
