//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/core/config.hpp>
#include <yave/module/std/core/decl/transform.hpp>

#include <yave/node/core/function.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/obj/mat/mat.hpp>

namespace yave {

  namespace modules::_std::core {

    struct TransformConstructor
      : NodeFunction<TransformConstructor, FrameDemand, FMat4>
    {
      return_type code() const
      {
        return make_object<FMat4>(eval_arg<0>()->matrix);
      }
    };

    struct TransformConstructor2
      : NodeFunction<TransformConstructor2, FMat4, FMat4>
    {
      return_type code() const
      {
        return arg<0>();
      }
    };
  } // namespace modules::_std::core

  template <>
  struct node_definition_traits<node::Transform, modules::_std::core::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::Transform>();

      auto d1 = node_definition(
        info.name(),
        0,
        make_object<
          InstanceGetterFunction<modules::_std::core::TransformConstructor>>(),
        info.description());

      auto d2 = node_definition(
        info.name(),
        0,
        make_object<
          InstanceGetterFunction<modules::_std::core::TransformConstructor2>>(),
        info.description());

      return {std::move(d1), std::move(d2)};
    }
  };
} // namespace yave