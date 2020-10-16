//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/color/color.hpp>
#include <yave/obj/color/property.hpp>
#include <yave/obj/primitive/property.hpp>

namespace yave {

  auto node_declaration_traits<node::Color::Color>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Color.Color",
      "Create Color",
      {"color"},
      {"color"},
      {{0, make_node_argument<Color>()}});
  }

  auto node_declaration_traits<node::Color::FromFloats>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Color.FromFloats",
      "Create Color from floats",
      {"R", "G", "B", "A"},
      {"color"},
      {{0, make_node_argument<Float>(0.f, 0.f, 1.f, 0.001f)},
       {1, make_node_argument<Float>(0.f, 0.f, 1.f, 0.001f)},
       {2, make_node_argument<Float>(0.f, 0.f, 1.f, 0.001f)},
       {3, make_node_argument<Float>(0.f, 0.f, 1.f, 0.001f)}});
  }

  namespace modules::_std::color {

    struct ColorCtor : NodeFunction<ColorCtor, Color, Color>
    {
      auto code() const -> return_type
      {
        return arg<0>();
      }
    };

    struct ColorFloat
      : NodeFunction<ColorFloat, Float, Float, Float, Float, Color>
    {
      auto code() const -> return_type
      {
        return make_object<Color>(
          *eval_arg<0>(), *eval_arg<1>(), *eval_arg<2>(), *eval_arg<3>());
      }
    };
  } // namespace modules::_std::color

  auto node_definition_traits<node::Color::Color, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Color::Color>();
    return std::vector {node_definition(
      info.full_name(), 0, make_object<modules::_std::color::ColorCtor>())};
  }

  auto node_definition_traits<node::Color::FromFloats, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Color::FromFloats>();
    return std::vector {node_definition(
      info.full_name(), 0, make_object<modules::_std::color::ColorFloat>())};
  }
} // namespace yave