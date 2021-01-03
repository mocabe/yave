//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/shape/merge.hpp>
#include <yave/lib/shape/shape.hpp> 
#include <yave/signal/function.hpp>
#include <yave/obj/shape/shape.hpp>

namespace yave {

  auto node_declaration_traits<node::Shape::Merge>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "Shape.Merge",
      "Merge two shapes",
      node_declaration_visibility::_public,
      {"src", "dst"},
      {"shape"});
  }

  namespace modules::_std::shape {

    struct MergeShape : SignalFunction<MergeShape, Shape, Shape, Shape>
    {
      auto code() const -> return_type
      {
        auto s1 = eval_arg<0>();
        auto s2 = eval_arg<1>();
        auto s  = s2.clone();
        s->merge(*s1);
        return s;
      }
    };
  } // namespace modules::_std::shape

  auto node_definition_traits<node::Shape::Merge, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::Shape::Merge>();
    return std::vector {node_definition(
      info.full_name(), 0, make_object<modules::_std::shape::MergeShape>())};
  }
}