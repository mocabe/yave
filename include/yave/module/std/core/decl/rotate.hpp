//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/core/decl/primitive.hpp>

namespace yave {

  namespace node {
    /// Rotate
    struct Rotate;
    /// RotateX
    struct RotateX;
    /// RotateY
    struct RotateY;
    /// RotateZ
    struct RotateZ;
  } // namespace node

  template <>
  struct node_declaration_traits<node::Rotate>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "Rotate",
        {"target", "deg", "axis"},
        {"out"},
        "Rotate object around axis",
        {{1, make_data_type_holder<Float>()}});
    }
  };

  template <>
  struct node_declaration_traits<node::RotateX>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "RotateX",
        {"target", "deg"},
        {"out"},
        "Rotate object around X axis",
        {{1, make_data_type_holder<Float>()}});
    }
  };

  template <>
  struct node_declaration_traits<node::RotateY>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "RotateY",
        {"target", "deg"},
        {"out"},
        "Rotate object around Y axis",
        {{1, make_data_type_holder<Float>()}});
    }
  };

  template <>
  struct node_declaration_traits<node::RotateZ>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "RotateZ",
        {"target", "deg"},
        {"out"},
        "Rotate object around Z axis",
        {{1, make_data_type_holder<Float>()}});
    }
  };
} // namespace yave