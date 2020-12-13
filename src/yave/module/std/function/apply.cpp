//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/function/apply.hpp>
#include <yave/signal/function.hpp>

namespace yave {

  auto node_declaration_traits<node::Functional::Apply>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Functional.Apply",
      "Apply argument to function",
      node_declaration_visibility::_public,
      {"fn", "arg"},
      {"fn"});
  }

  namespace modules::_std::functional {

    class X;
    class Y;

    struct Apply : Function<Apply, closure<X, Y>, X, Y>
    {
      return_type code() const
      {
        return arg<0>() << arg<1>();
      }
    };
  } // namespace modules::_std::functional

  auto node_definition_traits<node::Functional::Apply, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Functional::Apply>();
    return {node_definition(
      info.full_name(), 0, make_object<modules::_std::functional::Apply>())};
  }
} // namespace yave
