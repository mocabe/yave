//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/obj/primitive/property.hpp>
#include <yave/obj/color/color.hpp>

namespace yave {

  template <>
  struct node_argument_traits<Color>
  {
    static auto create_variable_members(data::color value)
    {
      return std::vector {
        make_node_argument_variable<Float>("r", value.r),
        make_node_argument_variable<Float>("g", value.g),
        make_node_argument_variable<Float>("b", value.b),
        make_node_argument_variable<Float>("a", value.a)};
    }

    static auto create_variable(std::string name, data::color value = {})
    {
      return make_object<NodeArgumentVariable>(
        std::move(name), object_type<Color>(), create_variable_members(value));
    }

    struct Generator : Function<Generator, NodeArgument, FrameDemand, Color>
    {
      auto code() const -> return_type
      {
        auto arg  = eval_arg<0>();
        auto type = arg->get_type();

        if (!same_type(type, object_type<Color>()))
          throw bad_value_cast(type, object_type<Color>());

        auto r = value_cast<Float>(arg->get_value("r"));
        auto g = value_cast<Float>(arg->get_value("g"));
        auto b = value_cast<Float>(arg->get_value("b"));
        auto a = value_cast<Float>(arg->get_value("a"));

        return make_object<Color>(*r, *g, *b, *a);
      }
    };

    static auto create(data::color value = {})
    {
      return make_object<NodeArgument>(
        object_type<Color>(),
        std::vector {create_variable_members(value)},
        make_object<Generator>());
    }
  };

} // namespace yave