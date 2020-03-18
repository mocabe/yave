//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/color/color.hpp>
#include <yave/obj/color/color.hpp>
#include <yave/module/std/primitive/data_holder.hpp>

namespace yave {

  auto node_declaration_traits<node::Color>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Color",
      "std::color",
      "Color",
      {"R", "G", "B", "A"},
      {"color"},
      {{0, make_data_type_holder<Float>()},
       {1, make_data_type_holder<Float>()},
       {2, make_data_type_holder<Float>()},
       {3, make_data_type_holder<Float>()}});
  }

  namespace modules::_std::color {

    struct ColorCtor
      : NodeFunction<ColorCtor, Float, Float, Float, Float, Color>
    {
      return_type code() const
      {
        return make_object<Color>(
          *eval_arg<0>(), *eval_arg<1>(), *eval_arg<2>(), *eval_arg<3>());
      }
    };
  } // namespace modules::_std::color

  auto node_definition_traits<node::Color, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Color>();
    return std::vector {node_definition(
      info.name(),
      0,
      make_object<modules::_std::color::ColorCtor>(),
      info.description())};
  }
}