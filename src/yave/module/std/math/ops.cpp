//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/math/ops.hpp>
#include <yave/signal/function.hpp>
#include <yave/signal/generator.hpp>

#include <functional>

namespace yave {

  namespace modules::_std::math {

    template <class T>
    using TAdd = BinaryFunction<T, T, T, std::plus<>>;

    template <class T>
    using TSub = BinaryFunction<T, T, T, std::minus<>>;

    template <class T>
    using TMul = BinaryFunction<T, T, T, std::multiplies<>>;

    template <class T>
    using TDiv = BinaryFunction<T, T, T, std::divides<>>;

  } // namespace modules::_std::math

  auto node_definition_traits<node::Ops::Add, modules::_std::math::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Ops::Add>();

    return {
      node_definition(
        info.full_name(), 0, make_object<modules::_std::math::TAdd<Float>>()),
      node_definition(
        info.full_name(), 0, make_object<modules::_std::math::TAdd<Int>>())};
  }

  auto node_definition_traits<node::Ops::Sub, modules::_std::math::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Ops::Sub>();

    return {
      node_definition(
        info.full_name(), 0, make_object<modules::_std::math::TSub<Float>>()),
      node_definition(
        info.full_name(), 0, make_object<modules::_std::math::TSub<Int>>())};
  }

  auto node_definition_traits<node::Ops::Mul, modules::_std::math::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Ops::Mul>();

    return {
      node_definition(
        info.full_name(), 0, make_object<modules::_std::math::TMul<Float>>()),
      node_definition(
        info.full_name(), 0, make_object<modules::_std::math::TMul<Int>>())};
  }

  auto node_definition_traits<node::Ops::Div, modules::_std::math::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Ops::Div>();

    return {
      node_definition(
        info.full_name(), 0, make_object<modules::_std::math::TDiv<Float>>()),
      node_definition(
        info.full_name(), 0, make_object<modules::_std::math::TDiv<Int>>())};
  }
} // namespace yave