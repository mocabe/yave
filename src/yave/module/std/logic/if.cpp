//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/logic/if.hpp>
#include <yave/node/core/function.hpp>

namespace yave {

  auto node_declaration_traits<node::If>::get_node_declaration()
    -> node_declaration
  {
    class X;
    return node_declaration(
      "If",
      "std::logic",
      "Basic control primitive. Takes single (Bool) value to switch call to "
      "subgraph. Because of lazy evaluation, subgraph not taken will not be "
      "evaluated",
      {"cond", "then", "else"},
      {"out"});
  }

  namespace modules::_std::logic {

    struct If_X;

    /// If
    struct If : NodeFunction<If, Bool, If_X, If_X, If_X>
    {
      return_type code() const
      {
        if (*eval_arg<0>())
          return arg<1>();
        else
          return arg<2>();
      }
    };

  } // namespace modules::_std::logic

  auto node_definition_traits<node::If, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::If>();
    return {node_definition(
      info.name(), 0, make_object<modules::_std::logic::If>(), info.name())};
  }
} // namespace yave