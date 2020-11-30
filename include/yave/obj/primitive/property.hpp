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
  requires                
    std::same_as<T, Int> || 
    std::same_as<T, Float>  
  struct node_argument_traits<T>
  {
    using value_type = typename T::value_type;

    /// \param value initial value
    /// \param min minimum value
    /// \param max max value
    /// \param step step of value change
    static auto create_variable(
      const std::string& name,
      value_type value = {0},
      value_type min   = std::numeric_limits<value_type>::lowest(),
      value_type max   = std::numeric_limits<value_type>::max(),
      value_type step  = {1})
    {
      auto val = node_argument_nvp {data::string(name), make_object<T>(value)};

      auto props = std::vector<node_argument_nvp> {
        {u8"min", make_object<T>(min)},
        {u8"max", make_object<T>(max)},
        {u8"step", make_object<T>(step)}};

      return make_object<NodeArgumentVariable>(val, props);
    }

    struct Generator : Function<Generator, NodeArgument, FrameDemand, T>
    {
      auto code() const -> typename Generator::return_type
      {
        auto arg = this->template eval_arg<0>();
        return value_cast<T>(arg->get_value("value"));
      }
    };

    static auto create(
      value_type value = {0},
      value_type min   = std::numeric_limits<value_type>::lowest(),
      value_type max   = std::numeric_limits<value_type>::max(),
      value_type step  = {1})
    {
      return make_object<NodeArgument>(
        object_type<T>(),
        std::vector {create_variable("value", value, min, max, step)},
        make_object<Generator>());
    }
  };

  // String
  template <>
  struct node_argument_traits<String>
  {
    static auto create_variable(
      const std::string& name,
      std::string value = "",
      std::string regex = R"(*)")
    {
      auto val =
        node_argument_nvp {data::string(name), make_object<String>(value)};

      auto props = std::vector<node_argument_nvp> {
        {u8"regex", make_object<String>(regex)}};

      return make_object<NodeArgumentVariable>(val, props);
    }

    struct Generator : Function<Generator, NodeArgument, FrameDemand, String>
    {
      auto code() const -> return_type
      {
        auto arg = eval_arg<0>();
        return value_cast<String>(arg->get_value("value"));
      }
    };

    static auto create(std::string value = "", std::string regex = R"(*)")
    {
      return make_object<NodeArgument>(
        object_type<String>(),
        std::vector {create_variable("value", value, regex)},
        make_object<Generator>());
    }
  };

  // Bool
  template <>
  struct node_argument_traits<Bool>
  {
    static auto create_variable(const std::string& name, bool value = false)
    {
      auto val =
        node_argument_nvp {data::string(name), make_object<Bool>(value)};

      auto props = std::vector<node_argument_nvp> {};

      return make_object<NodeArgumentVariable>(val, props);
    }

    struct Generator : Function<Generator, NodeArgument, FrameDemand, Bool>
    {
      auto code() const -> return_type
      {
        auto arg = eval_arg<0>();
        return value_cast<Bool>(arg->get_value("value"));
      }
    };

    static auto create(bool value = false)
    {
      return make_object<NodeArgument>(
        object_type<Bool>(),
        std::vector {create_variable("value", value)},
        make_object<Generator>());
    }
  };

} // namespace yave