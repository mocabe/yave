//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/geometry/mat.hpp>
#include <yave/module/std/transform/rotate.hpp>
#include <yave/module/std/transform/translate.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/mat/mat.hpp>
#include <yave/obj/vec/vec.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace yave {

  auto node_declaration_traits<node::Mat4>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Mat4", "/std/geometry", "4x4 Matrix", {}, {"value"});
  }

  auto node_declaration_traits<node::Mat4Rotate>::get_node_declaration()
    -> node_declaration
  {
    return yave::get_node_declaration<node::Rotate>();
  }

  auto node_declaration_traits<node::Mat4RotateX>::get_node_declaration()
    -> node_declaration
  {
    return yave::get_node_declaration<node::RotateX>();
  }

  auto node_declaration_traits<node::Mat4RotateY>::get_node_declaration()
    -> node_declaration
  {
    return yave::get_node_declaration<node::RotateY>();
  }

  auto node_declaration_traits<node::Mat4RotateZ>::get_node_declaration()
    -> node_declaration
  {
    return yave::get_node_declaration<node::RotateZ>();
  }

  auto node_declaration_traits<node::Mat4Translate>::get_node_declaration()
    -> node_declaration
  {
    return yave::get_node_declaration<node::Translate>();
  }

  namespace modules::_std::geometry {

    // Mat4 constructor
    struct Mat4Ctor : NodeFunction<Mat4Ctor, FMat4>
    {
      return_type code() const
      {
        return make_object<FMat4>(1);
      }
    };

    // Mat4 Rotate
    struct Mat4Rotate : NodeFunction<Mat4Rotate, FMat4, Float, FVec3, FMat4>
    {
      auto code() const -> return_type
      {
        auto transform = eval_arg<0>();
        auto angle     = eval_arg<1>();
        auto axis      = eval_arg<2>();

        if (*axis == fvec3(0, 0, 0))
          return arg<0>();

        auto m = glm::rotate(
          glm::mat4(1), glm::radians(*angle), glm::normalize(glm::vec3(*axis)));
        return make_object<FMat4>(m * *transform);
      }
    };

    // Mat4 RotateX
    struct Mat4RotateX : NodeFunction<Mat4RotateX, FMat4, Float, FMat4>
    {
      auto code() const -> return_type
      {
        auto transform = eval_arg<0>();
        auto angle     = eval_arg<1>();
        auto m =
          glm::rotate(glm::mat4(1), glm::radians(*angle), glm::vec3(1, 0, 0));
        return make_object<FMat4>(m * *transform);
      }
    };

    // Mat4 RotateY
    struct Mat4RotateY : NodeFunction<Mat4RotateY, FMat4, Float, FMat4>
    {
      auto code() const -> return_type
      {
        auto transform = eval_arg<0>();
        auto angle     = eval_arg<1>();
        auto m =
          glm::rotate(glm::mat4(1), glm::radians(*angle), glm::vec3(0, 1, 0));
        return make_object<FMat4>(*transform * m);
      }
    };

    // Mat4 RotateZ
    struct Mat4RotateZ : NodeFunction<Mat4RotateZ, FMat4, Float, FMat4>
    {
      auto code() const -> return_type
      {
        auto transform = eval_arg<0>();
        auto angle     = eval_arg<1>();
        auto m =
          glm::rotate(glm::mat4(1), glm::radians(*angle), glm::vec3(0, 0, 1));
        return make_object<FMat4>(m * *transform);
      }
    };

    // Mat4 Transform
    struct Mat4Translate : NodeFunction<Mat4Translate, FMat4, FVec3, FMat4>
    {
      auto code() const -> return_type
      {
        auto t = eval_arg<0>();
        auto v = eval_arg<1>();
        auto m = glm::translate(glm::mat4(1), glm::vec3(v->x, v->y, v->z));
        return make_object<FMat4>(m * *t);
      }
    };

  } // namespace modules::_std::geometry

  auto node_definition_traits<node::Mat4, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Mat4>();

    return {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::geometry::Mat4Ctor>(),
      info.description())};
  }

  auto node_definition_traits<node::Mat4Rotate, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Mat4Rotate>();

    return {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::geometry::Mat4Rotate>(),
      info.description())};
  }

  auto node_definition_traits<node::Mat4RotateX, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Mat4RotateX>();

    return {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::geometry::Mat4RotateX>(),
      info.description())};
  }

  auto node_definition_traits<node::Mat4RotateY, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Mat4RotateY>();

    return {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::geometry::Mat4RotateY>(),
      info.description())};
  }

  auto node_definition_traits<node::Mat4RotateZ, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Mat4RotateZ>();

    return {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::geometry::Mat4RotateZ>(),
      info.description())};
  }

  auto node_definition_traits<node::Mat4Translate, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Mat4Translate>();

    return {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::geometry::Mat4Translate>(),
      info.description())};
  }
} // namespace yave