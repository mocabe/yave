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
    static auto create_prop_tree(
      const std::string& name,
      data::color value = {})
    {
      return make_object<PropertyTreeNode>(
        name,
        object_type<Color>(),
        std::vector {
          make_object<PropertyTreeNode>("r", make_object<Float>(value.r)),
          make_object<PropertyTreeNode>("g", make_object<Float>(value.g)),
          make_object<PropertyTreeNode>("b", make_object<Float>(value.b)),
          make_object<PropertyTreeNode>("a", make_object<Float>(value.a))});
    }

    struct Generator : Function<Generator, NodeArgument, FrameDemand, Color>
    {
      auto code() const -> return_type
      {
        auto arg  = eval_arg<0>();
        auto tree = arg->property();

        assert(same_type(tree->type(), object_type<Color>()));

        auto cs = tree->children();
        auto r  = cs.at(0)->get_value<Float>();
        auto g  = cs.at(1)->get_value<Float>();
        auto b  = cs.at(2)->get_value<Float>();
        auto a  = cs.at(3)->get_value<Float>();

        return make_object<Color>(*r, *g, *b, *a);
      }
    };

    static auto create(data::color value = {})
    {
      return make_object<NodeArgument>(
        create_prop_tree("color", value), make_object<Generator>());
    }

    struct property_value
    {
      data::color value;
    };

    static auto get_value(const object_ptr<const PropertyTreeNode>& p)
    {
      auto ret    = property_value();
      auto cs     = p->children();
      ret.value.r = *cs.at(0)->get_value<Float>();
      ret.value.g = *cs.at(1)->get_value<Float>();
      ret.value.b = *cs.at(2)->get_value<Float>();
      ret.value.a = *cs.at(3)->get_value<Float>();
      return ret;
    }

    static auto set_value(
      const object_ptr<PropertyTreeNode>& p,
      data::color val)
    {
      auto cs = p->children();
      cs.at(0)->set_value(make_object<Float>(val.r));
      cs.at(1)->set_value(make_object<Float>(val.g));
      cs.at(2)->set_value(make_object<Float>(val.b));
      cs.at(3)->set_value(make_object<Float>(val.a));
    }

    static auto get_diff(
      const object_ptr<PropertyTreeNode>& p,
      data::color val)
    {
      auto cs  = p->children();
      auto ret = std::vector<node_argument_diff>();

      auto r = cs.at(0);
      auto g = cs.at(1);
      auto b = cs.at(2);
      auto a = cs.at(3);

      if (*r->get_value<Float>() != val.r)
        ret.push_back({r, make_object<Float>(val.r)});
      if (*r->get_value<Float>() != val.g)
        ret.push_back({g, make_object<Float>(val.g)});
      if (*r->get_value<Float>() != val.b)
        ret.push_back({b, make_object<Float>(val.b)});
      if (*r->get_value<Float>() != val.a)
        ret.push_back({a, make_object<Float>(val.a)});
      return ret;
    }
  };

} // namespace yave