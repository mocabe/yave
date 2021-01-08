//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/maybe/maybe.hpp>
#include <yave/signal/function.hpp>
#include <yave/signal/maybe.hpp>

namespace yave {

  auto node_declaration_traits<node::Maybe::Just>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Maybe.Just",
      "Create Maybe from value",
      node_declaration_visibility::_public,
      {"v"},
      {"m"});
  }

  auto node_declaration_traits<node::Maybe::Nothing>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Maybe.Nothing",
      "Create empty Maybe",
      node_declaration_visibility::_public,
      {},
      {"m"});
  }

  auto node_declaration_traits<node::Maybe::IsJust>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Maybe.IsJust",
      "Check if Maybe contains value",
      node_declaration_visibility::_public,
      {},
      {"b"});
  }

  auto node_declaration_traits<node::Maybe::IsNothing>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Maybe.IsNothing",
      "Check if Maybe contains not value",
      node_declaration_visibility::_public,
      {},
      {"b"});
  }

  auto node_declaration_traits<node::Maybe::GetValue>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Maybe.GetValue",
      "Get value in maybe",
      node_declaration_visibility::_public,
      {"m"},
      {"v"});
  }

  auto node_declaration_traits<node::Maybe::Map>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Maybe.Map",
      "Map value in Maybe\n"
      "\n"
      "param:\n"
      " m : Value of Maybe a\n"
      " fn: Mapping function of (a -> b)\n"
      "return:\n"
      " Maybe value of b",
      node_declaration_visibility::_public,
      {"m", "fn"},
      {"m"});
  }

  namespace node::Maybe::detail {

    class X;
    class Y;

    struct Just : SignalFunction<Just, X, SMaybe<X>>
    {
      auto code() const -> return_type
      {
        return make_object<SMaybe<X>>(arg_signal<0>());
      }
    };

    struct Nothing : SignalFunction<Nothing, SMaybe<X>>
    {
      auto code() const -> return_type
      {
        return make_object<SMaybe<X>>();
      }
    };

    struct IsJust : SignalFunction<IsJust, SMaybe<X>, Bool>
    {
      auto code() const -> return_type
      {
        return make_object<Bool>(eval_arg<0>()->has_value());
      }
    };

    struct IsNothing : SignalFunction<IsNothing, SMaybe<X>, Bool>
    {
      auto code() const -> return_type
      {
        return make_object<Bool>(eval_arg<0>()->is_nothing());
      }
    };

    struct GetValue : SignalFunction<GetValue, SMaybe<X>, X>
    {
      auto code() const -> return_type
      {
        return eval_arg<0>()->value() << arg_demand();
      }
    };

    struct Map : SignalFunction<Map, SMaybe<X>, sf<X, Y>, SMaybe<Y>>
    {
      auto code() const -> return_type
      {
        auto m = eval_arg<0>();

        if (m->is_nothing())
          return make_object<SMaybe<Y>>();

        return make_object<SMaybe<Y>>(arg_signal<1>() << m->value());
      }
    };

  } // namespace node::Maybe::detail

#define MAYBE_NODE_DEFINITION(NAME)                                    \
  auto node_definition_traits<node::Maybe::NAME, modules::_std::tag>:: \
    get_node_definitions()                                             \
      ->std::vector<node_definition>                                   \
  {                                                                    \
    auto info = get_node_declaration<node::Maybe::NAME>();             \
    return {node_definition(                                           \
      info.full_name(), 0, make_object<node::Maybe::detail::NAME>())}; \
  }

  MAYBE_NODE_DEFINITION(Just);
  MAYBE_NODE_DEFINITION(Nothing);
  MAYBE_NODE_DEFINITION(IsJust);
  MAYBE_NODE_DEFINITION(IsNothing);
  MAYBE_NODE_DEFINITION(GetValue);
  MAYBE_NODE_DEFINITION(Map);

} // namespace yave