//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/default/config.hpp>
#include <yave/node/decl/control_flow.hpp>
#include <yave/node/decl/constructor.hpp>

namespace yave {

  namespace backends::default_common {

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

  } // namespace backends::default_common

  template <>
  struct node_definition_traits<node::If, backend_tags::default_common>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::If>();
      return {node_definition(
        info.name(),
        info.output_sockets()[0],
        make_object<InstanceGetterFunction<backends::default_common::If>>(),
        info.name())};
    }
  };
} // namespace yave