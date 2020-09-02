//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/shape/transform.hpp>
#include <yave/module/std/primitive/primitive.hpp>
#include <yave/obj/shape/shape.hpp>
#include <yave/obj/vec/vec.hpp>
#include <yave/obj/mat/mat.hpp>
#include <yave/node/core/function.hpp>

namespace yave {

  auto node_declaration_traits<node::ShapeTranslate>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Translate",
      "/std/shape",
      "Translate shape",
      {"shape", "vec"},
      {"shape"});
  }

  auto node_declaration_traits<node::ShapeRotate>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Rotate",
      "/std/shape",
      "Rotate shape",
      {"shape", "degree", "center"},
      {"shape"},
      {{1, make_data_type_holder<Float>(0.f)}});
  }

  auto node_declaration_traits<node::ShapeScale>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Scale",
      "/std/shape",
      "Scale shape",
      {"shape", "ratio", "center"},
      {"shape"},
      {{1, make_data_type_holder<Float>(1.f)}});
  }

  namespace modules::_std::shape {

    struct Translate : NodeFunction<Translate, Shape, Vec2, Shape>
    {
      auto code() const -> return_type
      {
        auto s = eval_arg<0>();
        auto v = eval_arg<1>();

        auto ss = *s;
        ss.translate(v->x, v->y);

        return make_object<Shape>(std::move(ss));
      }
    };

    struct Rotate : NodeFunction<Rotate, Shape, Float, Vec2, Shape>
    {
      auto code() const -> return_type
      {
        auto s = eval_arg<0>();
        auto d = eval_arg<1>();
        auto v = eval_arg<2>();

        auto ss = *s;
        ss.rotate(*d, *v);

        return make_object<Shape>(std::move(ss));
      }
    };

    struct Scale : NodeFunction<Scale, Shape, Float, Vec2, Shape>
    {
      auto code() const -> return_type
      {
        auto s = eval_arg<0>();
        auto d = eval_arg<1>();
        auto v = eval_arg<2>();

        auto ss = *s;
        ss.scale(*d, *d, *v);

        return make_object<Shape>(std::move(ss));
      }
    };
  } // namespace modules::_std::shape

  auto node_definition_traits<node::ShapeTranslate, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::ShapeTranslate>();

    return {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::shape::Translate>(),
      info.description())};
  }

  auto node_definition_traits<node::ShapeRotate, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::ShapeRotate>();

    return {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::shape::Rotate>(),
      info.description())};
  }

  auto node_definition_traits<node::ShapeScale, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::ShapeScale>();

    return {node_definition(
      info.qualified_name(),
      0,
      make_object<modules::_std::shape::Scale>(),
      info.description())};
  }
} // namespace yave