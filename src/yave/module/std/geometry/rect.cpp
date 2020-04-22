//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/geometry/rect.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/vec/vec.hpp>
#include <yave/obj/rect/rect.hpp>

namespace yave {

  auto node_declaration_traits<node::Rect2>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Rect2", "std/geometry", "2D rectangle", {"off", "ext"}, {"value"});
  }

  auto node_declaration_traits<node::Rect3>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Rect3", "std/geometry", "3D rectangle", {"off", "ext"}, {"value"});
  }

  namespace modules::_std::geometry {

    // Rect2 constructor
    struct Rect2Ctor : NodeFunction<Rect2Ctor, FVec2, FVec2, Rect2>
    {
      return_type code() const
      {
        return make_object<Rect2>(*eval_arg<0>(), *eval_arg<1>());
      }
    };

    // Rect3 constructor
    struct Rect3Ctor : NodeFunction<Rect3Ctor, FVec3, FVec3, Rect3>
    {
      return_type code() const
      {
        return make_object<Rect3>(*eval_arg<0>(), *eval_arg<1>());
      }
    };
  } // namespace modules::_std::geometry

  auto node_definition_traits<node::Rect2, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Rect2>();

    return std::vector {node_definition(
      info.name(),
      0,
      make_object<modules::_std::geometry::Rect2Ctor>(),
      info.description())};
  }

  auto node_definition_traits<node::Rect3, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Rect3>();

    return std::vector {node_definition(
      info.name(),
      0,
      make_object<modules::_std::geometry::Rect3Ctor>(),
      info.description())};
  }
} // namespace yave
