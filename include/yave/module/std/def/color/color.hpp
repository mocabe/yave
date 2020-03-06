//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/decl/color/color.hpp>
#include <yave/module/std/config.hpp>
#include <yave/node/core/function.hpp>

namespace yave {

  namespace modules::_std::color {

    struct ColorCtor
      : NodeFunction<ColorCtor, Float, Float, Float, Float, Color>
    {
      return_type code() const
      {
        return make_object<Color>(
          eval_arg<0>(), eval_arg<1>(), eval_arg<2>(), eval_arg<3>());
      }
    };
  } // namespace modules::_std::color

  template <>
  struct node_definition_traits<node::Color, modules::_std::tag>
  {
    static auto get_node_definitions()
    {
      auto info = get_node_declaration<node::Color>();
      return std::vector {node_definition(
        info.name(),
        0,
        make_object<modules::_std::color::ColorCtor>(),
        info.description())};
    }
  };
} // namespace yave