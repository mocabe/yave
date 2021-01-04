//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/shape/shape.hpp>
#include <yave/obj/shape/shape.hpp>
#include <yave/lib/shape/rect.hpp>
#include <yave/lib/shape/circle.hpp>
#include <yave/obj/color/color.hpp>
#include <yave/obj/vec/property.hpp>
#include <yave/obj/primitive/property.hpp>
#include <yave/signal/function.hpp>

namespace yave {

  auto node_declaration_traits<node::Shape::Shape>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Shape.Shape",
      "Create empty shape",
      node_declaration_visibility::_public,
      {},
      {"shape"});
  }

  auto node_declaration_traits<node::Shape::Rect>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Shape.Rectangle",
      "Create rect shape",
      node_declaration_visibility::_public,
      {"pos", "size"},
      {"shape"},
      {{0, make_node_argument<Vec2>()}, {1, make_node_argument<Vec2>()}});
  }

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

    struct Rect : SignalFunction<Rect, Vec2, Vec2, Shape>
    {
      auto code() const -> return_type
      {
        auto pos  = eval_arg<0>();
        auto size = eval_arg<1>();
        return make_object<Shape>(create_rect_shape(*pos, *size));
      }
    };

    struct Circle : SignalFunction<Circle, Vec2, Float, Shape>
    {
      auto code() const -> return_type
      {
        auto pos = eval_arg<0>();
        auto rad = eval_arg<1>();
        return make_object<Shape>(create_circle_shape(*pos, *rad));
      }
    };

    struct Shape : SignalFunction<Shape, yave::Shape>
    {
      auto code() const -> return_type
      {
        return make_object<yave::Shape>(yave::shape());
      }
    };

  } // namespace modules::_std::shape

#define SHAPE_NODE_DEFINITION(NAME)                                     \
  auto node_definition_traits<node::Shape::NAME, modules::_std::tag>::  \
    get_node_definitions()                                              \
      ->std::vector<node_definition>                                    \
  {                                                                     \
    auto info = get_node_declaration<node::Shape::NAME>();              \
    return {node_definition(                                            \
      info.full_name(), 0, make_object<modules::_std::shape::NAME>())}; \
  }

  SHAPE_NODE_DEFINITION(Shape);
  SHAPE_NODE_DEFINITION(Rect);
  SHAPE_NODE_DEFINITION(Circle);

} // namespace yave