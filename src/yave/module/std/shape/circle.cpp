//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/shape/circle.hpp>
#include <yave/lib/shape/circle.hpp>
#include <yave/obj/shape/shape.hpp>
#include <yave/obj/color/color.hpp>
#include <yave/obj/vec/property.hpp>
#include <yave/obj/primitive/property.hpp>

namespace yave {

  auto node_declaration_traits<node::Shape::Circle>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Shape.Circle",
      "Create circle shape",
      node_declaration_visibility::_public,
      {"pos", "radius"},
      {"shape"},
      {{0, make_node_argument<Vec2>()}, {1, make_node_argument<Float>(128.f)}});
  }

  namespace modules::_std::shape {

    struct CircleShapeCtor : NodeFunction<CircleShapeCtor, Vec2, Float, Shape>
    {
      auto code() const -> return_type
      {
        auto pos = eval_arg<0>();
        auto rad = eval_arg<1>();
        return make_object<Shape>(create_circle_shape(*pos, *rad));
      }
    };
  } // namespace modules::_std::shape

  auto node_definition_traits<node::Shape::Circle, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Shape::Circle>();
    return std::vector {node_definition(
      get_full_name(info),
      0,
      make_object<modules::_std::shape::CircleShapeCtor>())};
  }

} // namespace yave