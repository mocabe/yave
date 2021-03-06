//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/logic/if.hpp>
#include <yave/signal/function.hpp>

namespace yave {

  auto node_declaration_traits<node::Logic::If>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Logic.If",
      "Basic control primitive. \n"
      "Takes single (Bool) value to switch call to subgraph",
      node_declaration_visibility::_public,
      {"cond", "then", "else"},
      {"out"});
  }

  namespace modules::_std::logic {

    class X;

    struct If : SignalFunction<If, Bool, X, X, X>
    {
      auto code() const -> return_type
      {
        if (*eval_arg<0>())
          return arg<1>();
        else
          return arg<2>();
      }
    };

  } // namespace modules::_std::logic

  auto node_definition_traits<node::Logic::If, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Logic::If>();
    return {node_definition(
      info.full_name(), 0, make_object<modules::_std::logic::If>())};
  }
} // namespace yave