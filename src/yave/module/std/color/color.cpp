//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/color/color.hpp>
#include <yave/obj/color/color.hpp>
#include <yave/module/std/primitive/primitive.hpp>

namespace yave {

  auto node_declaration_traits<node::Color>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Color",
      "/std/color",
      "Color",
      {"color"},
      {"color"},
      {{0, make_data_type_holder<Color>()}});
  }

  auto node_declaration_traits<node::ColorFloat>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "ColorFloat",
      "/std/color",
      "Color",
      {"R", "G", "B", "A"},
      {"color"},
      {{0, make_data_type_holder<Float>(0.f, 0.f, 1.f, 0.001f)},
       {1, make_data_type_holder<Float>(0.f, 0.f, 1.f, 0.001f)},
       {2, make_data_type_holder<Float>(0.f, 0.f, 1.f, 0.001f)},
       {3, make_data_type_holder<Float>(0.f, 0.f, 1.f, 0.001f)}});
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
          static_cast<float>(*eval_arg<0>()),
          static_cast<float>(*eval_arg<1>()),
          static_cast<float>(*eval_arg<2>()),
          static_cast<float>(*eval_arg<3>()));
      }
    };
  } // namespace modules::_std::color

  auto node_definition_traits<node::Color, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Color>();
    return std::vector {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::color::ColorCtor>(),
      info.description())};
  }

  auto node_definition_traits<node::ColorFloat, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::ColorFloat>();
    return std::vector {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::color::ColorFloat>(),
      info.description())};
  }
} // namespace yave