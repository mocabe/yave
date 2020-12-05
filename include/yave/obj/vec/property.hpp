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
      return make_object<PropertyTreeNode>(
        name,
        object_type<Vec2>(),
        std::vector {
          make_object<PropertyTreeNode>("x", make_object<Float>(value.x)),
          make_object<PropertyTreeNode>("y", make_object<Float>(value.y))});
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
        auto x  = cs.at(0)->get_value<Float>();
        auto y  = cs.at(1)->get_value<Float>();

        return make_object<Vec2>(*x, *y);
      }
    };

    static auto create(glm::vec2 value = {})
    {
      return make_object<NodeArgument>(
        create_prop_tree("", value), make_object<Generator>());
    }

    struct property_value
    {
      glm::vec2 value;
    };

    static auto get_value(const object_ptr<const PropertyTreeNode>& p)
    {
      auto ret    = property_value();
      auto cs     = p->children();
      ret.value.x = *cs.at(0)->get_value<Float>();
      ret.value.y = *cs.at(1)->get_value<Float>();
      return ret;
    }

    static auto set_value(
      const object_ptr<PropertyTreeNode>& p,
      data::vec2 val)
    {
      auto cs = p->children();
      cs.at(0)->set_value(make_object<const Float>(val.x));
      cs.at(1)->set_value(make_object<const Float>(val.y));
    }

    static auto get_diff(
      const object_ptr<PropertyTreeNode>& p,
      data::vec2 val)
    {
      auto cs  = p->children();
      auto ret = std::vector<node_argument_diff>();

      auto x = cs.at(0);
      auto y = cs.at(1);

      if (*x->get_value<Float>() != val.x)
        ret.push_back({x, make_object<Float>(val.x)});
      if (*y->get_value<Float>() != val.y)
        ret.push_back({y, make_object<Float>(val.y)});
      return ret;
    }
  };

} // namespace yave
