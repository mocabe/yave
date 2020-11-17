//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/math/trigonometric.hpp>
#include <yave/node/core/function.hpp>
#include <yave/node/core/generator.hpp>

#include <cmath>

namespace yave {

  auto node_declaration_traits<node::Math::Sin>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Math.Trig.Sin",
      "sin",
      node_declaration_visibility::_public,
      {"x"},
      {"sin(x)"});
  }

  auto node_declaration_traits<node::Math::Cos>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Math.Trig.Cos",
      "sin",
      node_declaration_visibility::_public,
      {"x"},
      {"cos(x)"});
  }

  auto node_declaration_traits<node::Math::Tan>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Math.Trig.Tan",
      "sin",
      node_declaration_visibility::_public,
      {"x"},
      {"tan(x)"});
  }

  namespace modules::_std::math {

    auto tsin = [](auto x) { return std::sin(x); };
    auto tcos = [](auto x) { return std::cos(x); };
    auto ttan = [](auto x) { return std::tan(x); };

    template <class T>
    using TSin = UnaryFunction<T, T, decltype(tsin)>;

    template <class T>
    using TCos = UnaryFunction<T, T, decltype(tcos)>;

    template <class T>
    using TTan = UnaryFunction<T, T, decltype(ttan)>;

  } // namespace modules::_std::math

  auto node_definition_traits<node::Math::Sin, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Math::Sin>();
    return {node_definition(
      info.full_name(), 0, make_object<modules::_std::math::TSin<Float>>())};
  }

  auto node_definition_traits<node::Math::Cos, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Math::Cos>();
    return {node_definition(
      info.full_name(), 0, make_object<modules::_std::math::TCos<Float>>())};
  }

  auto node_definition_traits<node::Math::Tan, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Math::Tan>();
    return {node_definition(
      info.full_name(), 0, make_object<modules::_std::math::TTan<Float>>())};
  }
}