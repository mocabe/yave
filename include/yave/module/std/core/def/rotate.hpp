//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/module/std/core/config.hpp>
#include <yave/module/std/core/decl/rotate.hpp>
#include <yave/node/core/function.hpp>

#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/obj/primitive/primitive.hpp>
#include <yave/obj/mat/mat.hpp>
#include <yave/obj/vec/vec.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace yave {

  namespace modules::_std::core {

    class Rotate_X;
    class RotateX_X;
    class RotateY_X;
    class RotateZ_X;

    struct Rotate : Function<
                      Rotate,
                      node_closure<forall<Rotate_X>>,
                      node_closure<Float>, /* angle */
                      node_closure<FVec3>, /* axis */
                      FrameDemand,
                      forall<Rotate_X>>
    {
      auto code() const -> return_type
      {
        auto fd    = eval_arg<3>();
        auto angle = eval(arg<1>() << fd);
        auto axis  = eval(arg<2>() << fd);

        // avoid nan
        if (*axis == fvec3(0, 0, 0))
          return arg<0>() << fd;

        auto m = glm::rotate(
          glm::mat4(1), glm::radians(*angle), glm::normalize(glm::vec3(*axis)));

        return arg<0>() << make_object<FrameDemand>(fd->time, fd->matrix * m);
      }
    };

    struct RotateX : Function<
                       RotateX,
                       node_closure<forall<RotateX_X>>,
                       node_closure<Float>, /* angle */
                       FrameDemand,
                       forall<RotateX_X>>
    {
      auto code() const -> return_type
      {
        auto fd    = eval_arg<2>();
        auto angle = eval(arg<1>() << fd);
        auto m =
          glm::rotate(glm::mat4(1), glm::radians(*angle), glm::vec3(1, 0, 0));
        return arg<0>() << make_object<FrameDemand>(fd->time, fd->matrix * m);
      }
    };

    struct RotateY : Function<
                       RotateY,
                       node_closure<forall<RotateY_X>>,
                       node_closure<Float>, /* angle */
                       FrameDemand,
                       forall<RotateY_X>>
    {
      auto code() const -> return_type
      {
        auto fd    = eval_arg<2>();
        auto angle = eval(arg<1>() << fd);
        auto m =
          glm::rotate(glm::mat4(1), glm::radians(*angle), glm::vec3(0, 1, 0));
        return arg<0>() << make_object<FrameDemand>(fd->time, fd->matrix * m);
      }
    };

    struct RotateZ : Function<
                       RotateZ,
                       node_closure<forall<RotateZ_X>>,
                       node_closure<Float>, /* angle */
                       FrameDemand,
                       forall<RotateZ_X>>
    {
      auto code() const -> return_type
      {
        auto fd    = eval_arg<2>();
        auto angle = eval(arg<1>() << fd);
        auto m =
          glm::rotate(glm::mat4(1), glm::radians(*angle), glm::vec3(0, 0, 1));
        return arg<0>() << make_object<FrameDemand>(fd->time, fd->matrix * m);
      }
    };
  } // namespace modules::_std::core

  template <>
  struct node_definition_traits<node::Rotate, modules::_std::core::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::Rotate>();

      return {node_definition(
        info.name(),
        0,
        make_object<InstanceGetterFunction<modules::_std::core::Rotate>>(),
        info.description())};
    }
  };

  template <>
  struct node_definition_traits<node::RotateX, modules::_std::core::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::RotateX>();

      return {node_definition(
        info.name(),
        0,
        make_object<InstanceGetterFunction<modules::_std::core::RotateX>>(),
        info.description())};
    }
  };

  template <>
  struct node_definition_traits<node::RotateY, modules::_std::core::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::RotateY>();

      return {node_definition(
        info.name(),
        0,
        make_object<InstanceGetterFunction<modules::_std::core::RotateY>>(),
        info.description())};
    }
  };

  template <>
  struct node_definition_traits<node::RotateZ, modules::_std::core::tag>
  {
    static auto get_node_definitions() -> std::vector<node_definition>
    {
      auto info = get_node_declaration<node::RotateZ>();

      return {node_definition(
        info.name(),
        0,
        make_object<InstanceGetterFunction<modules::_std::core::RotateZ>>(),
        info.description())};
    }
  };
} // namespace yave