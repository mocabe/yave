//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/shape/transform.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/shape/shape.hpp>
#include <yave/obj/vec/vec.hpp>
#include <yave/obj/mat/mat.hpp>
#include <yave/obj/primitive/property.hpp>

namespace yave {

  auto node_declaration_traits<node::Shape::Translate>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Shape.Translate", "Translate shape", {"shape", "vec"}, {"shape"});
  }

  auto node_declaration_traits<node::Shape::Rotate>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Shape.Rotate",
      "Rotate shape",
      {"shape", "degree", "center"},
      {"shape"},
      {{1, make_node_argument<Float>(0.f)}});
  }

  auto node_declaration_traits<node::Shape::Scale>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Shape.Scale",
      "Scale shape",
      {"shape", "ratio", "center"},
      {"shape"},
      {{1, make_node_argument<Float>(1.f)}});
  }

  namespace modules::_std::shape {

    struct Translate : NodeFunction<Translate, Shape, Vec2, Shape>
    {
      auto code() const -> return_type
      {
        auto s = eval_arg<0>();
        auto v = eval_arg<1>();

        auto ss = yave::shape(*s);
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

        auto ss = yave::shape(*s);
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

        auto ss = yave::shape(*s);
        ss.scale(*d, *d, *v);

        return make_object<Shape>(std::move(ss));
      }
    };
  } // namespace modules::_std::shape

  auto node_definition_traits<node::Shape::Translate, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Shape::Translate>();

    return {node_definition(
      info.full_name(), 0, make_object<modules::_std::shape::Translate>())};
  }

  auto node_definition_traits<node::Shape::Rotate, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Shape::Rotate>();

    return {node_definition(
      info.full_name(), 0, make_object<modules::_std::shape::Rotate>())};
  }

  auto node_definition_traits<node::Shape::Scale, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Shape::Scale>();

    return {node_definition(
      info.full_name(), 0, make_object<modules::_std::shape::Scale>())};
  }
} // namespace yave