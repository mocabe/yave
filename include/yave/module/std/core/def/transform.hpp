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
    /// FMat4 -> FMat4
    struct TransformConstructor
      : NodeFunction<TransformConstructor, FMat4, FMat4>
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

      return {node_definition(
        info.name(),
        0,
        make_object<modules::_std::core::TransformConstructor>(),
        info.description())};
    }
  };
} // namespace yave