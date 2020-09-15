//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/shape/rect.hpp>
#include <yave/lib/shape/rect.hpp>
#include <yave/obj/shape/shape.hpp>
#include <yave/obj/color/color.hpp>
#include <yave/obj/vec/property.hpp>

namespace yave {

  auto node_declaration_traits<node::RectShape>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Rectangle",
      "/std/shape",
      "Create rect shape",
      {"pos", "size"},
      {"shape"},
      {{0, make_node_argument<Vec2>()}, {1, make_node_argument<Vec2>()}});
  }

  namespace modules::_std::shape {

    struct RectShapeCtor : NodeFunction<RectShapeCtor, Vec2, Vec2, Shape>
    {
      auto code() const -> return_type
      {
        auto pos  = eval_arg<0>();
        auto size = eval_arg<1>();
        return make_object<Shape>(create_rect_shape(*pos, *size));
      }
    };
  } // namespace modules::_std::shape

  auto node_definition_traits<node::RectShape, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::RectShape>();
    return std::vector {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::shape::RectShapeCtor>(),
      info.description())};
  }

} // namespace yave