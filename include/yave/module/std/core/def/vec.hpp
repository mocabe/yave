//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/core/decl/vec.hpp>
#include <yave/obj/vec/vec.hpp>

namespace yave {

  namespace modules::_std::core {

    struct Vec2Constructor : NodeFunction<Vec2Constructor, Float, Float, FVec2>
    {
      return_type code() const
      {
        return make_object<FVec2>(*eval_arg<0>(), *eval_arg<1>());
      }
    };

    struct Vec3Constructor
      : NodeFunction<Vec3Constructor, Float, Float, Float, FVec3>
    {
      return_type code() const
      {
        return make_object<FVec3>(
          *eval_arg<0>(), *eval_arg<1>(), *eval_arg<2>());
      }
    };

    struct Vec4Constructor
      : NodeFunction<Vec4Constructor, Float, Float, Float, Float, FVec4>
    {
      return_type code() const
      {
        return make_object<FVec4>(
          *eval_arg<0>(), *eval_arg<1>(), *eval_arg<2>(), *eval_arg<3>());
      }
    };
  } // namespace modules::_std::core

  template <>
  struct node_definition_traits<node::Vec2, modules::_std::core::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::Vec2>();

      return {node_definition(
        info.name(),
        0,
        make_object<
          InstanceGetterFunction<modules::_std::core::Vec2Constructor>>(),
        info.description())};
    }
  };

  template <>
  struct node_definition_traits<node::Vec3, modules::_std::core::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::Vec3>();

      return {node_definition(
        info.name(),
        0,
        make_object<
          InstanceGetterFunction<modules::_std::core::Vec3Constructor>>(),
        info.description())};
    }
  };

  template <>
  struct node_definition_traits<node::Vec4, modules::_std::core::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::Vec4>();

      return {node_definition(
        info.name(),
        0,
        make_object<
          InstanceGetterFunction<modules::_std::core::Vec4Constructor>>(),
        info.description())};
    }
  };
} // namespace yave