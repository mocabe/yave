//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/core/decl/bind.hpp>
#include <yave/node/core/function.hpp>

namespace yave {

  namespace modules::_std::core {

    namespace detail {
      class Bind_X;
      class Bind_Y;
    } // namespace detail

    struct Bind : Function<
                    Bind,
                    closure<detail::Bind_X, detail::Bind_Y>,
                    detail::Bind_X,
                    detail::Bind_Y>
    {
      return_type code() const
      {
        return arg<0>() << arg<1>();
      }
    };
  } // namespace modules::_std::core

  template <>
  struct node_definition_traits<node::Bind, modules::_std::core::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::Bind>();

      return {node_definition(
        info.name(),
        0,
        make_object<modules::_std::core::Bind>(),
        info.description())};
    }
  };
} // namespace yave
