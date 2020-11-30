//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/obj/primitive/property.hpp>
#include <yave/obj/vec/vec.hpp>

namespace yave {

  template <>
  struct node_argument_traits<Vec2>
  {
    static auto create_variable_members(glm::vec2 value)
    {
      return std::vector {
        make_node_argument_variable<Float>("x", value.x),
        make_node_argument_variable<Float>("y", value.y)};
    }

    static auto create_variable(glm::vec2 value = {})
    {
      return make_object<NodeArgumentVariable>(
        "vec2", object_type<Vec2>(), create_variable_members(value));
    }

    struct Generator : Function<Generator, NodeArgument, Vec2>
    {
      auto code() const -> return_type
      {
        auto arg  = eval_arg<0>();
        auto type = arg->get_type();

        if (!same_type(type, object_type<Vec2>()))
          throw bad_value_cast(type, object_type<Vec2>());

        auto x = value_cast<Float>(arg->get_value("x"));
        auto y = value_cast<Float>(arg->get_value("y"));

        return make_object<Vec2>(*x, *y);
      }
    };

    static auto create(glm::vec2 value = {})
    {
      return make_object<NodeArgument>(
        object_type<Vec2>(),
        std::vector {create_variable_members(value)},
        make_object<Generator>());
    }
  };

} // namespace yave
