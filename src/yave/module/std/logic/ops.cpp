//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/logic/ops.hpp>
#include <yave/signal/generator.hpp>

namespace yave {

  auto node_declaration_traits<node::Logic::And>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Logic.And",
      "Logical AND operation\n",
      node_declaration_visibility::_public,
      {"b1", "b2"},
      {"b"});
  }

  auto node_declaration_traits<node::Logic::Or>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Logic.Or",
      "Logical OR operation\n",
      node_declaration_visibility::_public,
      {"b1", "b2"},
      {"b"});
  }

  auto node_declaration_traits<node::Logic::Xor>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Logic.Xor",
      "Logical XOR operation\n",
      node_declaration_visibility::_public,
      {"b1", "b2"},
      {"b"});
  }

  auto node_declaration_traits<node::Logic::Not>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Logic.Not",
      "Logical NOT operation\n",
      node_declaration_visibility::_public,
      {"b"},
      {"b"});
  }

  namespace node::Logic::detail {

    constexpr auto and_fn = [](auto b1, auto b2) { return b1 && b2; };
    constexpr auto or_fn  = [](auto b1, auto b2) { return b1 || b2; };
    constexpr auto xor_fn = [](auto b1, auto b2) { return b1 != b2; };
    constexpr auto not_fn = [](auto b) { return !b; };

    using And = BinarySignalFunction<Bool, Bool, Bool, decltype(and_fn)>;
    using Or  = BinarySignalFunction<Bool, Bool, Bool, decltype(or_fn)>;
    using Xor = BinarySignalFunction<Bool, Bool, Bool, decltype(xor_fn)>;
    using Not = UnarySignalFunction<Bool, Bool, decltype(not_fn)>;

  } // namespace node::Logic::detail

#define LOGIC_NODE_DEFINITION(NAME)                                    \
  auto node_definition_traits<node::Logic::NAME, modules::_std::tag>:: \
    get_node_definitions()                                             \
      ->std::vector<node_definition>                                   \
  {                                                                    \
    auto info = get_node_declaration<node::Logic::NAME>();             \
    return {node_definition(                                           \
      info.full_name(), 0, make_object<node::Logic::detail::NAME>())}; \
  }

  LOGIC_NODE_DEFINITION(And);
  LOGIC_NODE_DEFINITION(Or);
  LOGIC_NODE_DEFINITION(Xor);
  LOGIC_NODE_DEFINITION(Not);

} // namespace yave