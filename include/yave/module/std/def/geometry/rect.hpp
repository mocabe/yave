//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/decl/geometry/rect.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/vec/vec.hpp>
#include <yave/module/std/config.hpp>

namespace yave {

  namespace modules::_std::geometry {
    struct Rect2Ctor : NodeFunction<Rect2Ctor, FVec2, FVec2, Rect2>
    {
      return_type code() const
      {
        return make_object<Rect2>(*eval_arg<0>(), *eval_arg<1>());
      }
    };

    struct Rect3Ctor : NodeFunction<Rect3Ctor, FVec3, FVec3, Rect3>
    {
      return_type code() const
      {
        return make_object<Rect3>(*eval_arg<0>(), *eval_arg<1>());
      }
    };
  } // namespace modules::_std::geometry

  template <>
  struct node_definition_traits<node::Rect2, modules::_std::tag>
  {
    static auto get_node_definitions()
    {
      auto info = get_node_declaration<node::Rect2>();

      return std::vector {node_definition(
        info.name(),
        0,
        make_object<modules::_std::geometry::Rect2Ctor>(),
        info.description())};
    }
  };

  template <>
  struct node_definition_traits<node::Rect3, modules::_std::tag>
  {
    static auto get_node_definitions()
    {
      auto info = get_node_declaration<node::Rect3>();

      return std::vector {node_definition(
        info.name(),
        0,
        make_object<modules::_std::geometry::Rect3Ctor>(),
        info.description())};
    }
  };
} // namespace yave
