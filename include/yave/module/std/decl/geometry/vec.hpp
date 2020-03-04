//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/core/get_info.hpp>
#include <yave/node/core/function.hpp>
#include <yave/module/std/decl/prim/primitive.hpp>
#include <yave/lib/vec/vec.hpp>

namespace yave {

  namespace node {
    /// float vec2
    struct Vec2;
    /// float vec3
    struct Vec3;
    /// float vec4
    struct Vec4;
  } // namespace node

  template <>
  struct node_declaration_traits<node::Vec2>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "Vec2",
        "Vec2 constructor",
        "std::geometry",
        {"x", "y"},
        {"value"},
        {{0, make_data_type_holder<Float>()},
         {1, make_data_type_holder<Float>()}});
    }
  };

  template <>
  struct node_declaration_traits<node::Vec3>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "Vec3",
        "Vec3 constructor",
        "std::geometry",
        {"x", "y", "z"},
        {"value"},
        {{0, make_data_type_holder<Float>()},
         {1, make_data_type_holder<Float>()},
         {2, make_data_type_holder<Float>()}});
    }
  };

  template <>
  struct node_declaration_traits<node::Vec4>
  {
    static auto get_node_declaration() -> node_declaration
    {
      return node_declaration(
        "Vec4",
        "Vec3 constructor",
        "std::geometry",
        {"x", "y", "z", "w"},
        {"value"},
        {{0, make_data_type_holder<Float>()},
         {1, make_data_type_holder<Float>()},
         {2, make_data_type_holder<Float>()},
         {3, make_data_type_holder<Float>()}});
    }
  };
}