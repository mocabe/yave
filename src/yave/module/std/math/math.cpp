//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/math/math.hpp>
#include <yave/node/core/function.hpp>
#include <yave/node/core/generator.hpp>

#include <functional>

namespace yave {

  auto node_declaration_traits<node::MathAdd>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration("Add", "/std/math", "a + b", {"a", "b"}, {"a + b"});
  }

  auto node_declaration_traits<node::MathSubtract>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Subtract", "/std/math", "a - b", {"a", "b"}, {"a - b"});
  }

  auto node_declaration_traits<node::MathMultiply>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Multiply", "/std/math", "a * b", {"a", "b"}, {"a * b"});
  }

  auto node_declaration_traits<node::MathDivide>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Divide", "/std/math", "a / b", {"a", "b"}, {"a / b"});
  }

  namespace modules::_std::math {

    template <class T>
    using TAdd = BinaryFunction<T, T, T, std::plus<>>;

    template <class T>
    using TSubtract = BinaryFunction<T, T, T, std::minus<>>;

    template <class T>
    using TMultiply = BinaryFunction<T, T, T, std::multiplies<>>;

    template <class T>
    using TDivide = BinaryFunction<T, T, T, std::divides<>>;

  } // namespace modules::_std::math

  auto node_definition_traits<node::MathAdd, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::MathAdd>();

    return {
      node_definition(
        info.qualified_name(),
        0,
        make_object<modules::_std::math::TAdd<Float>>(),
        info.name()),
      node_definition(
        info.qualified_name(),
        0,
        make_object<modules::_std::math::TAdd<Int>>(),
        info.name()),
    };
  }

  auto node_definition_traits<node::MathSubtract, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::MathSubtract>();

    return {
      node_definition(
        info.qualified_name(),
        0,
        make_object<modules::_std::math::TSubtract<Float>>(),
        info.name()),
      node_definition(
        info.qualified_name(),
        0,
        make_object<modules::_std::math::TSubtract<Int>>(),
        info.name())};
  }

  auto node_definition_traits<node::MathMultiply, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::MathMultiply>();

    return {
      node_definition(
        info.qualified_name(),
        0,
        make_object<modules::_std::math::TMultiply<Float>>(),
        info.name()),
      node_definition(
        info.qualified_name(),
        0,
        make_object<modules::_std::math::TMultiply<Int>>(),
        info.name())};
  }

  auto node_definition_traits<node::MathDivide, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::MathDivide>();

    return {
      node_definition(
        info.qualified_name(),
        0,
        make_object<modules::_std::math::TDivide<Float>>(),
        info.name()),
      node_definition(
        info.qualified_name(),
        0,
        make_object<modules::_std::math::TDivide<Int>>(),
        info.name())};
  }
}