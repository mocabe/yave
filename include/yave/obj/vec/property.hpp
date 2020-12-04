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
    static auto create_prop_tree(const std::string& name, glm::vec2 value = {})
    {
      return make_object<NodeArgumentPropNode>(
        name,
        object_type<Vec2>(),
        std::vector {
          make_object<NodeArgumentPropNode>("x", make_object<Float>(value.x)),
          make_object<NodeArgumentPropNode>("y", make_object<Float>(value.y))});
    }

    struct Generator : Function<Generator, NodeArgument, FrameDemand, Vec2>
    {
      auto code() const -> return_type
      {
        auto arg  = eval_arg<0>();
        auto tree = arg->prop_tree();

        assert(same_type(tree->type(), object_type<Vec2>()));
        assert(tree->children()[0]->name() == "x");
        assert(tree->children()[1]->name() == "y");

        auto cs = tree->children();
        auto x  = value_cast<Float>(cs.at(0)->value());
        auto y  = value_cast<Float>(cs.at(1)->value());

        return make_object<Vec2>(*x, *y);
      }
    };

    static auto create(glm::vec2 value = {})
    {
      return make_object<NodeArgument>(
        create_prop_tree("", value), make_object<Generator>());
    }

    static auto get_value(const object_ptr<const NodeArgumentPropNode>& p)
    {
      auto cs = p->children();
      auto x  = value_cast<Float>(cs.at(0)->value());
      auto y  = value_cast<Float>(cs.at(1)->value());
      return data::vec2(*x, *y);
    }

    static auto get_diff(
      const object_ptr<NodeArgumentPropNode>& p,
      data::vec2 val)
    {
      auto cs  = p->children();
      auto ret = std::vector<node_argument_diff>();

      auto x = cs.at(0);
      auto y = cs.at(1);

      if (*value_cast<Float>(x->value()) != val.x)
        ret.push_back({x, make_object<Float>(val.x)});
      if (*value_cast<Float>(y->value()) != val.y)
        ret.push_back({y, make_object<Float>(val.y)});
      return ret;
    }

    static auto set_value(
      const object_ptr<NodeArgumentPropNode>& p,
      data::vec2 val)
    {
      auto cs = p->children();
      cs.at(0)->set_value(make_object<Float>(val.x));
      cs.at(1)->set_value(make_object<Float>(val.y));
    }
  };

} // namespace yave
