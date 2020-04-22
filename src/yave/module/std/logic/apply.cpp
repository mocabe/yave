//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/logic/apply.hpp>
#include <yave/node/core/function.hpp>

namespace yave {

  auto node_declaration_traits<node::Apply>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Apply",
      "std/logic",
      "Apply argument to function",
      {"fn", "arg"},
      {"fn"});
  }

  namespace modules::_std::logic {

    class Apply_X;
    class Apply_Y;

    struct Apply : Function<Apply, closure<Apply_X, Apply_Y>, Apply_X, Apply_Y>
    {
      return_type code() const
      {
        return arg<0>() << arg<1>();
      }
    };
  } // namespace modules::_std::logic

  auto node_definition_traits<node::Apply, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Apply>();

    return {node_definition(
      info.name(),
      0,
      make_object<modules::_std::logic::Apply>(),
      info.description())};
  }
} // namespace yave
