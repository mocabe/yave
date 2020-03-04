//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/core/decl/if.hpp>

namespace yave {

  namespace modules::_std::core {

    namespace detail {
      struct If_X;
    }

    /// If
    struct If : NodeFunction<If, Bool, detail::If_X, detail::If_X, detail::If_X>
    {
      return_type code() const
      {
        if (*eval_arg<0>())
          return arg<1>();
        else
          return arg<2>();
      }
    };

  } // namespace modules::_std::core

  template <>
  struct node_definition_traits<node::If, modules::_std::core::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::If>();
      return {node_definition(
        info.name(),
        0,
        make_object<modules::_std::core::If>(),
        info.name())};
    }
  };
} // namespace yave 