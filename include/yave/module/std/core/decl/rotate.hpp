//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/module/std/core/decl/primitive.hpp>
#include <yave/module/std/core/decl/transform.hpp>

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
        "Rotate object around axis",
        "std::core::geometry",
        {"target", "deg", "axis", "transform"},
        {"out"},
        {{1, make_data_type_holder<Float>()},
         {3, make_object<modules::_std::core::GetCurrentTransform>()}});
    }
  };

  template <>
  struct node_declaration_traits<node::RotateX>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "RotateX",
        "Rotate object around X axis",
        "std::core::geometry",
        {"target", "deg", "transform"},
        {"out"},
        {{1, make_data_type_holder<Float>()},
         {2, make_object<modules::_std::core::GetCurrentTransform>()}});
    }
  };

  template <>
  struct node_declaration_traits<node::RotateY>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "RotateY",
        "Rotate object around Y axis",
        "std::core::geometry",
        {"target", "deg", "transform"},
        {"out"},
        {{1, make_data_type_holder<Float>()},
         {2, make_object<modules::_std::core::GetCurrentTransform>()}});
    }
  };

  template <>
  struct node_declaration_traits<node::RotateZ>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "RotateZ",
        "Rotate object around Z axis",
        "std::core::geometry",
        {"target", "deg", "transform"},
        {"out"},
        {{1, make_data_type_holder<Float>()},
         {2, make_object<modules::_std::core::GetCurrentTransform>()}});
    }
  };
} // namespace yave