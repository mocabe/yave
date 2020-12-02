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
        make_node_argument_prop_tree<Float>("r", value.r),
        make_node_argument_prop_tree<Float>("g", value.g),
        make_node_argument_prop_tree<Float>("b", value.b),
        make_node_argument_prop_tree<Float>("a", value.a)};
    }

    static auto create_prop_tree(
      const std::string& name,
      data::color value = {})
    {
      return make_object<NodeArgumentPropNode>(
        name, object_type<Color>(), create_variable_members(value));
    }

    struct Generator : Function<Generator, NodeArgument, FrameDemand, Color>
    {
      auto code() const -> return_type
      {
        auto arg  = eval_arg<0>();
        auto tree = arg->prop_tree();

        assert(same_type(tree->type(), object_type<Color>()));

        auto cs = tree->children();
        auto r  = value_cast<Float>(cs.at(0)->value());
        auto g  = value_cast<Float>(cs.at(1)->value());
        auto b  = value_cast<Float>(cs.at(2)->value());
        auto a  = value_cast<Float>(cs.at(3)->value());

        return make_object<Color>(*r, *g, *b, *a);
      }
    };

    static auto create(data::color value = {})
    {
      return make_object<NodeArgument>(
        create_prop_tree("color", value), make_object<Generator>());
    }

    static auto get_value(const object_ptr<const NodeArgumentPropNode>& p)
    {
      auto cs = p->children();
      auto r  = value_cast<Float>(cs.at(0)->value());
      auto g  = value_cast<Float>(cs.at(1)->value());
      auto b  = value_cast<Float>(cs.at(2)->value());
      auto a  = value_cast<Float>(cs.at(3)->value());
      return data::color {*r, *g, *b, *a};
    }

    static auto get_diff(
      const object_ptr<NodeArgumentPropNode>& p,
      data::color val)
    {
      auto cs  = p->children();
      auto ret = std::vector<node_argument_diff>();

      auto r = cs.at(0);
      auto g = cs.at(1);
      auto b = cs.at(2);
      auto a = cs.at(3);

      if (*value_cast<Float>(r->value()) != val.r)
        ret.push_back({r, make_object<Float>(val.r)});
      if (*value_cast<Float>(g->value()) != val.g)
        ret.push_back({g, make_object<Float>(val.g)});
      if (*value_cast<Float>(b->value()) != val.b)
        ret.push_back({b, make_object<Float>(val.b)});
      if (*value_cast<Float>(a->value()) != val.a)
        ret.push_back({a, make_object<Float>(val.a)});
      return ret;
    }

    static auto set_value(
      const object_ptr<NodeArgumentPropNode>& p,
      data::color val)
    {
      auto cs = p->children();
      cs.at(0)->set_value(make_object<Float>(val.r));
      cs.at(1)->set_value(make_object<Float>(val.g));
      cs.at(2)->set_value(make_object<Float>(val.b));
      cs.at(3)->set_value(make_object<Float>(val.a));
    }
  };

} // namespace yave