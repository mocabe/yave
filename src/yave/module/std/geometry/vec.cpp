//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/geometry/vec.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/vec/property.hpp>
#include <yave/obj/primitive/property.hpp>

namespace yave {

  auto node_declaration_traits<node::Vec2>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Vec2",
      "/std/geometry",
      "Vec2 constructor",
      {"x", "y"},
      {"value"},
      {{0, make_node_argument<Float64>()}, {1, make_node_argument<Float64>()}});
  }

  auto node_declaration_traits<node::Vec3>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Vec3",
      "/std/geometry",
      "Vec3 constructor",
      {"x", "y", "z"},
      {"value"},
      {{0, make_node_argument<Float>()},
       {1, make_node_argument<Float>()},
       {2, make_node_argument<Float>()}});
  }

  auto node_declaration_traits<node::Vec4>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Vec4",
      "/std/geometry",
      "Vec3 constructor",
      {"x", "y", "z", "w"},
      {"value"},
      {{0, make_node_argument<Float>()},
       {1, make_node_argument<Float>()},
       {2, make_node_argument<Float>()},
       {3, make_node_argument<Float>()}});
  }

  namespace modules::_std::geometry {

    struct Vec2Constructor : NodeFunction<Vec2Constructor, Float, Float, Vec2>
    {
      return_type code() const
      {
        return make_object<Vec2>(*eval_arg<0>(), *eval_arg<1>());
      }
    };

    struct Vec3Constructor
      : NodeFunction<Vec3Constructor, Float, Float, Float, Vec3>
    {
      return_type code() const
      {
        return make_object<Vec3>(
          *eval_arg<0>(), *eval_arg<1>(), *eval_arg<2>());
      }
    };

    struct Vec4Constructor
      : NodeFunction<Vec4Constructor, Float, Float, Float, Float, Vec4>
    {
      return_type code() const
      {
        return make_object<Vec4>(
          *eval_arg<0>(), *eval_arg<1>(), *eval_arg<2>(), *eval_arg<3>());
      }
    };
  } // namespace modules::_std::geometry

  auto node_definition_traits<node::Vec2, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Vec2>();

    return {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::geometry::Vec2Constructor>(),
      info.description())};
  }

  auto node_definition_traits<node::Vec3, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Vec3>();

    return {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::geometry::Vec3Constructor>(),
      info.description())};
  }

  auto node_definition_traits<node::Vec4, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Vec4>();

    return {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::geometry::Vec4Constructor>(),
      info.description())};
  }
} // namespace yave