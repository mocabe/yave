//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/shape/circle.hpp>
#include <yave/module/std/primitive/primitive.hpp>
#include <yave/obj/shape/shape.hpp>
#include <yave/lib/shape/circle.hpp>
#include <yave/obj/color/color.hpp>
#include <yave/obj/vec/vec.hpp>

namespace yave {

  auto node_declaration_traits<node::CircleShape>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Circle",
      "/std/shape",
      "Create circle shape",
      {"pos", "radius"},
      {"shape"},
      {{1, make_data_type_holder<Float>(128.f)}});
  }

  namespace modules::_std::shape {

    struct CircleShapeCtor : NodeFunction<CircleShapeCtor, FVec2, Float, Shape>
    {
      auto code() const -> return_type
      {
        auto pos = eval_arg<0>();
        auto rad = eval_arg<1>();
        return make_object<Shape>(create_circle_shape(*pos, *rad));
      }
    };
  } // namespace modules::_std::shape

  auto node_definition_traits<node::CircleShape, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::CircleShape>();
    return std::vector {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::shape::CircleShapeCtor>(),
      info.description())};
  }

} // namespace yave