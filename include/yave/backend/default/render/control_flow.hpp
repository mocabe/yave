//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/backend/default/config.hpp>
#include <yave/node/class/control_flow.hpp>
#include <yave/node/class/constructor.hpp>

namespace yave {

  namespace backend::default_render {

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

  } // namespace backend::default_render

  template <>
  struct node_definition_traits<node::If, backend::tags::default_render>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::If>();
      return {node_definition(
        info.name(),
        info.output_sockets()[0],
        make_object<InstanceGetterFunction<backend::default_render::If>>(),
        info.name())};
    }
  };
} // namespace yave