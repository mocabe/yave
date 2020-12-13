//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/function/fix.hpp>
#include <yave/signal/function.hpp>
#include <yave/rts/fix.hpp>

namespace yave {

  auto node_declaration_traits<node::Functional::Fix>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Functional.Fix",
      "Fixed point combinator",
      node_declaration_visibility::_public,
      {"fn"},
      {"fn"});
  }

  namespace modules::_std::functional {

    class X;

    struct Fix : Function<Fix, closure<X, X>, X>
    {
      auto code() const -> return_type
      {
        return make_object<yave::Fix>() << arg<0>();
      }
    };
  } // namespace modules::_std::functional

  auto node_definition_traits<node::Functional::Fix, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Functional::Fix>();
    return {node_definition(
      info.full_name(), 0, make_object<modules::_std::functional::Fix>())};
  }

} // namespace yave