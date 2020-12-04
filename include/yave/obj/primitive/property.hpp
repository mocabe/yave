//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/obj/primitive/primitive.hpp>
#include <yave/obj/node/argument.hpp>
#include <yave/rts/function.hpp>

#include <limits>
#include <concepts>

namespace yave {

  // Float, Int
  template <class T>
    requires std::same_as<T, Int> || std::same_as<T, Float>  
  struct node_argument_traits<T>
  {
    using value_type = typename T::value_type;

    struct property_value
    {
      value_type value = {};
      value_type min   = {};
      value_type max   = {};
      value_type step  = {};
    };

    static constexpr auto value_name = "value";
    static constexpr auto min_name   = "min";
    static constexpr auto max_name   = "max";
    static constexpr auto step_name  = "step";

    /// \param value initial value
    /// \param min minimum value
    /// \param max max value
    /// \param step step of value change
    static auto create_prop_tree(
      const std::string& name,
      value_type value = {0},
      value_type min   = std::numeric_limits<value_type>::lowest(),
      value_type max   = std::numeric_limits<value_type>::max(),
      value_type step  = {1})
    {
      return make_object<NodeArgumentPropNode>(
        name,
        object_type<T>(),
        std::vector {
          make_object<NodeArgumentPropNode>(value_name, make_object<T>(value)),
          make_object<NodeArgumentPropNode>(min_name, make_object<T>(min)),
          make_object<NodeArgumentPropNode>(max_name, make_object<T>(max)),
          make_object<NodeArgumentPropNode>(step_name, make_object<T>(step))});
    }

    struct Generator : Function<Generator, NodeArgument, FrameDemand, T>
    {
      auto code() const -> typename Generator::return_type
      {
        auto arg  = this->template eval_arg<0>();
        auto tree = arg->prop_tree();

        for (auto&& c : tree->children())
          if (c->name() == value_name)
            return value_cast<T>(c->value());

        throw std::runtime_error("invalid argument property");
      }
    };

    static auto create(
      value_type value = {0},
      value_type min   = std::numeric_limits<value_type>::lowest(),
      value_type max   = std::numeric_limits<value_type>::max(),
      value_type step  = {1})
    {
      return make_object<NodeArgument>(
        create_prop_tree("", value, min, max, step), make_object<Generator>());
    }

    static auto get_value(const object_ptr<const NodeArgumentPropNode>& arg)
      -> property_value
    {
      auto ret = property_value();

      for (auto&& c : arg->children()) {

        auto name = c->name();

        /**/ if (name == value_name)
          ret.value = *value_cast<T>(c->value());
        else if (name == min_name)
          ret.min = *value_cast<T>(c->value());
        else if (name == max_name)
          ret.max = *value_cast<T>(c->value());
        else if (name == step_name)
          ret.step = *value_cast<T>(c->value());
      }
      return ret;
    }

    static void set_value(
      const object_ptr<NodeArgumentPropNode>& arg,
      value_type val)
    {
      for (auto&& c : arg->children()) {
        if (c->name() == value_name) {
          c->set_value(make_object<T>(val));
          return;
        }
      }
    }

    static auto get_diff(
      const object_ptr<NodeArgumentPropNode>& arg,
      value_type val)
    {
      auto ret = std::vector<node_argument_diff>();
      for (auto&& c : arg->children()) {
        if (c->name() == value_name) {
          if (*value_cast<T>(c->value()) != val)
            ret.push_back({c, make_object<T>(val)});
        }
      }
      return ret;
    }
  };

  // String, Bool
  template <class T> 
    requires std::same_as<T, Bool> || std::same_as<T, String>
  struct node_argument_traits<T>
  {
    using value_type = typename T::value_type;

    struct property_value
    {
      value_type value;
    };

    static constexpr auto value_name = "value";

    static auto create_prop_tree(const std::string& name, value_type value = {})
    {
      return make_object<NodeArgumentPropNode>(
        name,
        object_type<T>(),
        std::vector {make_object<NodeArgumentPropNode>(
          value_name, make_object<T>(value))});
    }

    struct Generator : Function<Generator, NodeArgument, FrameDemand, T>
    {
      auto code() const -> typename Generator::return_type
      {
        auto arg = this->template eval_arg<0>();
        return value_cast<T>(arg->prop_tree()->value());
      }
    };

    static auto create(value_type value = {})
    {
      return make_object<NodeArgument>(
        create_prop_tree("", value), make_object<Generator>());
    }

    static auto get_value(const object_ptr<const NodeArgumentPropNode>& arg)
      -> property_value
    {
      auto ret = property_value();

      for (auto&& c : arg->children()) {
        if (c->name() == value_name) {
          ret.value = *value_cast<T>(c->value());
        }
      }
      return ret;
    }

    static void set_value(
      const object_ptr<NodeArgumentPropNode>& arg,
      value_type val)
    {
      for (auto&& c : arg->children()) {
        if (c->name() == value_name) {
          c->set_value(make_object<T>(val));
          return;
        }
      }
    }

    static auto get_diff(
      const object_ptr<NodeArgumentPropNode>& arg,
      value_type val)
    {
      auto ret = std::vector<node_argument_diff>();
      for (auto&& c : arg->children()) {
        if (c->name() == value_name) {
          if (*value_cast<T>(c->value()) != val)
            ret.push_back({c, make_object<T>(val)});
        }
      }
      return ret;
    }
  };

} // namespace yave