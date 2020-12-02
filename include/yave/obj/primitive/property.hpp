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

    static constexpr auto value_name = "value";
    static constexpr auto min_name   = u8"min";
    static constexpr auto max_name   = u8"max";
    static constexpr auto step_name  = u8"step";

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
      auto props = std::vector<node_argument_nvp> {
        {min_name, make_object<T>(min)},
        {max_name, make_object<T>(max)},
        {step_name, make_object<T>(step)}};

      return make_object<NodeArgumentPropNode>(
        name, make_object<T>(value), props);
    }

    struct Generator : Function<Generator, NodeArgument, FrameDemand, T>
    {
      auto code() const -> typename Generator::return_type
      {
        auto arg = this->template eval_arg<0>();
        return value_cast<T>(arg->prop_tree()->value());
      }
    };

    static auto create(
      value_type value = {0},
      value_type min   = std::numeric_limits<value_type>::lowest(),
      value_type max   = std::numeric_limits<value_type>::max(),
      value_type step  = {1})
    {
      return make_object<NodeArgument>(
        create_prop_tree(value_name, value, min, max, step),
        make_object<Generator>());
    }

    static auto get_value(const object_ptr<const NodeArgumentPropNode>& arg)
      -> value_type
    {
      return *value_cast<T>(arg->value());
    }

    static auto get_diff(
      const object_ptr<NodeArgumentPropNode>& arg,
      value_type val)
    {
      auto ret = std::vector<node_argument_diff>();
      if (*value_cast<T>(arg->value()) != val)
        ret.emplace_back(arg, make_object<T>(val));
      return ret;
    }

    static void set_value(
      const object_ptr<NodeArgumentPropNode>& arg,
      value_type val)
    {
      arg->set_value(make_object<T>(val));
    }
  };

  // String, Bool
  template <class T> 
    requires std::same_as<T, Bool> || std::same_as<T, String>
  struct node_argument_traits<T>
  {
    using value_type = typename T::value_type;

    static constexpr auto value_name = "value";

    static auto create_prop_tree(const std::string& name, value_type value = {})
    {
      return make_object<NodeArgumentPropNode>(
        name, make_object<T>(value), std::vector<node_argument_nvp>());
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
        create_prop_tree(value_name, value), make_object<Generator>());
    }

    static auto get_value(const object_ptr<const NodeArgumentPropNode>& arg)
      -> value_type
    {
      return *value_cast<T>(arg->value());
    }

    static auto get_diff(
      const object_ptr<NodeArgumentPropNode>& arg,
      value_type val)
    {
      auto ret = std::vector<node_argument_diff>();
      if (*value_cast<T>(arg->value()) != val)
        ret.emplace_back(arg, make_object<T>(val));
      return ret;
    }

    static void set_value(
      const object_ptr<NodeArgumentPropNode>& arg,
      value_type val)
    {
      arg->set_value(make_object<T>(val));
    }
  };

} // namespace yave