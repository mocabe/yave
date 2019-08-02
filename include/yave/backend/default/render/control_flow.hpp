//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/node/obj/control_flow.hpp>

#include <yave/node/obj/function.hpp>
#include <yave/data/obj/primitive.hpp>
#include <yave/backend/default/system/config.hpp>

namespace yave {

  namespace backend::default_render {

    namespace detail {
      struct IfNode_X;
    }

    /// If
    struct IfNode : NodeFunction<
                      IfNode,
                      Bool,
                      detail::IfNode_X,
                      detail::IfNode_X,
                      detail::IfNode_X>
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
  struct bind_info_traits<IfNode, backend::tags::default_render>
  {
    static bind_info get_bind_info()
    {
      return {
        "If",
        {"cond", "then", "else"},
        "out",
        make_object<InstanceGetterFunction<backend::default_render::IfNode>>(),
        "IfNode"};
    }
  };
} // namespace yave