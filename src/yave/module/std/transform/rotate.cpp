//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/transform/rotate.hpp>
#include <yave/module/std/transform/transform.hpp>
#include <yave/module/std/primitive/data_holder.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/frame_demand/frame_demand.hpp>
#include <yave/obj/primitive/primitive.hpp>
#include <yave/obj/mat/mat.hpp>
#include <yave/obj/vec/vec.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace yave {

  auto node_declaration_traits<node::Rotate>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Rotate",
      "std::transform",
      "Rotate object around axis",
      {"target", "deg", "axis", "transform"},
      {"out"},
      {{1, make_data_type_holder<Float>()},
       {3, make_object<modules::_std::core::GetCurrentTransform>()}});
  }

  auto node_declaration_traits<node::RotateX>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "RotateX",
      "std::transform",
      "Rotate object around X axis",
      {"target", "deg", "transform"},
      {"out"},
      {{1, make_data_type_holder<Float>()},
       {2, make_object<modules::_std::core::GetCurrentTransform>()}});
  }

  auto node_declaration_traits<node::RotateY>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "RotateY",
      "std::transform",
      "Rotate object around Y axis",
      {"target", "deg", "transform"},
      {"out"},
      {{1, make_data_type_holder<Float>()},
       {2, make_object<modules::_std::core::GetCurrentTransform>()}});
  }

  auto node_declaration_traits<node::RotateZ>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "RotateZ",
      "std::transform",
      "Rotate object around Z axis",
      {"target", "deg", "transform"},
      {"out"},
      {{1, make_data_type_holder<Float>()},
       {2, make_object<modules::_std::core::GetCurrentTransform>()}});
  }

  namespace modules::_std::geometry {

    class Rotate_X;
    class RotateX_X;
    class RotateY_X;
    class RotateZ_X;

    struct Rotate : Function<
                      Rotate,
                      node_closure<forall<Rotate_X>>,
                      node_closure<Float>, /* angle */
                      node_closure<FVec3>, /* axis */
                      node_closure<FMat4>, /* transform */
                      FrameDemand,
                      forall<Rotate_X>>
    {
      auto code() const -> return_type
      {
        auto fd        = eval_arg<4>();
        auto angle     = eval(arg<1>() << fd);
        auto axis      = eval(arg<2>() << fd);
        auto transform = eval(arg<3>() << fd);

        // avoid nan
        if (*axis == fvec3(0, 0, 0))
          return arg<0>() << fd;

        auto m = glm::rotate(
          glm::mat4(1), glm::radians(*angle), glm::normalize(glm::vec3(*axis)));
        return arg<0>() << make_object<FrameDemand>(fd->time, *transform * m);
      }
    };

    struct RotateX : Function<
                       RotateX,
                       node_closure<forall<RotateX_X>>,
                       node_closure<Float>, /* angle */
                       node_closure<FMat4>, /* transform */
                       FrameDemand,
                       forall<RotateX_X>>
    {
      auto code() const -> return_type
      {
        auto fd        = eval_arg<3>();
        auto angle     = eval(arg<1>() << fd);
        auto transform = eval(arg<2>() << fd);
        auto m =
          glm::rotate(glm::mat4(1), glm::radians(*angle), glm::vec3(1, 0, 0));
        return arg<0>() << make_object<FrameDemand>(fd->time, *transform * m);
      }
    };

    struct RotateY : Function<
                       RotateY,
                       node_closure<forall<RotateY_X>>,
                       node_closure<Float>, /* angle */
                       node_closure<FMat4>, /* transform */
                       FrameDemand,
                       forall<RotateY_X>>
    {
      auto code() const -> return_type
      {
        auto fd        = eval_arg<3>();
        auto angle     = eval(arg<1>() << fd);
        auto transform = eval(arg<2>() << fd);
        auto m =
          glm::rotate(glm::mat4(1), glm::radians(*angle), glm::vec3(0, 1, 0));
        return arg<0>() << make_object<FrameDemand>(fd->time, *transform * m);
      }
    };

    struct RotateZ : Function<
                       RotateZ,
                       node_closure<forall<RotateZ_X>>,
                       node_closure<Float>, /* angle */
                       node_closure<FMat4>, /* transform */
                       FrameDemand,
                       forall<RotateZ_X>>
    {
      auto code() const -> return_type
      {
        auto fd        = eval_arg<3>();
        auto angle     = eval(arg<1>() << fd);
        auto transform = eval(arg<2>() << fd);
        auto m =
          glm::rotate(glm::mat4(1), glm::radians(*angle), glm::vec3(0, 0, 1));
        return arg<0>() << make_object<FrameDemand>(fd->time, *transform * m);
      }
    };
  } // namespace modules::_std::geometry

  auto node_definition_traits<node::Rotate, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Rotate>();

    return {node_definition(
      info.name(),
      0,
      make_object<modules::_std::geometry::Rotate>(),
      info.description())};
  }

  auto node_definition_traits<node::RotateX, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::RotateX>();

    return {node_definition(
      info.name(),
      0,
      make_object<modules::_std::geometry::RotateX>(),
      info.description())};
  }

  auto node_definition_traits<node::RotateY, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::RotateY>();

    return {node_definition(
      info.name(),
      0,
      make_object<modules::_std::geometry::RotateY>(),
      info.description())};
  }

  auto node_definition_traits<node::RotateZ, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::RotateZ>();

    return {node_definition(
      info.name(),
      0,
      make_object<modules::_std::geometry::RotateZ>(),
      info.description())};
  }
} // namespace yave