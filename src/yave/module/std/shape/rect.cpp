//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/shape/rect.hpp>
#include <yave/lib/shape/rect.hpp>
#include <yave/obj/shape/shape.hpp>
#include <yave/obj/color/color.hpp>
#include <yave/obj/vec/property.hpp>
#include <yave/signal/function.hpp>

namespace yave {

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

  namespace modules::_std::shape {

    struct RectShapeCtor : SignalFunction<RectShapeCtor, Vec2, Vec2, Shape>
    {
      auto code() const -> return_type
      {
        auto pos  = eval_arg<0>();
        auto size = eval_arg<1>();
        return make_object<Shape>(create_rect_shape(*pos, *size));
      }
    };
  } // namespace modules::_std::shape

  auto node_definition_traits<node::Shape::Rect, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Shape::Rect>();
    return std::vector {node_definition(
      info.full_name(), 0, make_object<modules::_std::shape::RectShapeCtor>())};
  }

} // namespace yave