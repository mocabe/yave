//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/mat/mat.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/mat/mat.hpp>
#include <yave/obj/vec/vec.hpp>
#include <yave/obj/primitive/property.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace yave {

  auto node_declaration_traits<node::Mat::Mat4>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration("Std.Mat.Mat4", "4x4 Matrix", {}, {"mat4"});
  }

  auto node_declaration_traits<node::Mat::Rotate>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Std.Mat.Rotate",
      "Rotate object around axis",
      {"mat4", "deg", "axis"},
      {"mat4"},
      {{1, make_node_argument<Float>()}});
  }

  auto node_declaration_traits<node::Mat::RotateX>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Std.Mat.RotateX",
      "Rotate object around X axis",
      {"mat4", "deg"},
      {"mat4"},
      {{1, make_node_argument<Float>()}});
  }

  auto node_declaration_traits<node::Mat::RotateY>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Std.Mat.RotateY",
      "Rotate object around Y axis",
      {"mat4", "deg"},
      {"mat4"},
      {{1, make_node_argument<Float>()}});
  }

  auto node_declaration_traits<node::Mat::RotateZ>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Std.Mat.RotateZ",
      "Rotate object around Z axis",
      {"mat4", "deg"},
      {"mat4"},
      {{1, make_node_argument<Float>()}});
  }

  auto node_declaration_traits<node::Mat::Translate>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Std.Mat.Translate", "Move object", {"mat4", "vec3"}, {"mat4"});
  }

  namespace modules::_std::geometry {

    // Mat4 constructor
    struct Mat4Ctor : NodeFunction<Mat4Ctor, Mat4>
    {
      return_type code() const
      {
        return make_object<Mat4>(1);
      }
    };

    // Mat4 Rotate
    struct Mat4Rotate : NodeFunction<Mat4Rotate, Mat4, Float, Vec3, Mat4>
    {
      auto code() const -> return_type
      {
        auto transform = eval_arg<0>();
        auto angle     = eval_arg<1>();
        auto axis      = eval_arg<2>();

        if (*axis == glm::dvec3(0, 0, 0))
          return arg<0>();

        auto m = glm::rotate(
          glm::dmat4(1),
          glm::radians(*angle),
          glm::normalize(glm::dvec3(*axis)));
        return make_object<Mat4>(m * *transform);
      }
    };

    // Mat4 RotateX
    struct Mat4RotateX : NodeFunction<Mat4RotateX, Mat4, Float, Mat4>
    {
      auto code() const -> return_type
      {
        auto transform = eval_arg<0>();
        auto angle     = eval_arg<1>();
        auto m =
          glm::rotate(glm::dmat4(1), glm::radians(*angle), glm::dvec3(1, 0, 0));
        return make_object<Mat4>(m * *transform);
      }
    };

    // Mat4 RotateY
    struct Mat4RotateY : NodeFunction<Mat4RotateY, Mat4, Float, Mat4>
    {
      auto code() const -> return_type
      {
        auto transform = eval_arg<0>();
        auto angle     = eval_arg<1>();
        auto m =
          glm::rotate(glm::dmat4(1), glm::radians(*angle), glm::dvec3(0, 1, 0));
        return make_object<Mat4>(*transform * m);
      }
    };

    // Mat4 RotateZ
    struct Mat4RotateZ : NodeFunction<Mat4RotateZ, Mat4, Float, Mat4>
    {
      auto code() const -> return_type
      {
        auto transform = eval_arg<0>();
        auto angle     = eval_arg<1>();
        auto m =
          glm::rotate(glm::dmat4(1), glm::radians(*angle), glm::dvec3(0, 0, 1));
        return make_object<Mat4>(m * *transform);
      }
    };

    // Mat4 Transform
    struct Mat4Translate : NodeFunction<Mat4Translate, Mat4, Vec3, Mat4>
    {
      auto code() const -> return_type
      {
        auto t = eval_arg<0>();
        auto v = eval_arg<1>();
        auto m = glm::translate(glm::dmat4(1), glm::dvec3(v->x, v->y, v->z));
        return make_object<Mat4>(m * *t);
      }
    };

  } // namespace modules::_std::geometry

  auto node_definition_traits<node::Mat::Mat4, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Mat::Mat4>();

    return {node_definition(
      info.full_name(), 0, make_object<modules::_std::geometry::Mat4Ctor>())};
  }

  auto node_definition_traits<node::Mat::Rotate, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Mat::Rotate>();

    return {node_definition(
      info.full_name(), 0, make_object<modules::_std::geometry::Mat4Rotate>())};
  }

  auto node_definition_traits<node::Mat::RotateX, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Mat::RotateX>();

    return {node_definition(
      info.full_name(),
      0,
      make_object<modules::_std::geometry::Mat4RotateX>())};
  }

  auto node_definition_traits<node::Mat::RotateY, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Mat::RotateY>();

    return {node_definition(
      info.full_name(),
      0,
      make_object<modules::_std::geometry::Mat4RotateY>())};
  }

  auto node_definition_traits<node::Mat::RotateZ, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Mat::RotateZ>();

    return {node_definition(
      info.full_name(),
      0,
      make_object<modules::_std::geometry::Mat4RotateZ>())};
  }

  auto node_definition_traits<node::Mat::Translate, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Mat::Translate>();

    return {node_definition(
      info.full_name(),
      0,
      make_object<modules::_std::geometry::Mat4Translate>())};
  }
} // namespace yave