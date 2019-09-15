//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/obj/control_flow.hpp>

#include <yave/node/core/function.hpp>
#include <yave/node/obj/constructor.hpp>
#include <yave/backend/default/system/config.hpp>

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
  struct bind_info_traits<node::If, backend::tags::default_render>
  {
    static bind_info get_bind_info()
    {
      auto info = get_node_info<node::If>();
      return {
        info.name(),
        info.input_sockets(),
        info.output_sockets()[0],
        make_object<InstanceGetterFunction<backend::default_render::If>>(),
        info.name()};
    }
  };
} // namespace yave