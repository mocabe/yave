//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/primitive/primitive.hpp>
#include <yave/module/std/num/num.hpp>
#include <yave/module/std/string/string.hpp>
#include <yave/module/std/logic/bool.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/primitive/property.hpp>

namespace yave {

  template <class T>
  auto node_declaration_traits<node::PrimitiveCtor<T>>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      name,
      "Data type constructor",
      node_declaration_visibility::_public,
      {"value"},
      {"value"},
      {{0, make_node_argument<T>()}});
  }

  namespace modules::_std::prim {

    template <class T>
    struct PrimitiveCtor : NodeFunction<PrimitiveCtor<T>, T, T>
    {
      auto code() const -> typename PrimitiveCtor::return_type
      {
        return PrimitiveCtor::template eval_arg<0>();
      }
    };

  } // namespace modules::_std::prim

  template <class T>
  auto node_definition_traits<node::PrimitiveCtor<T>, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::PrimitiveCtor<T>>();
    return {node_definition(
      info.full_name(),
      0,
      make_object<modules::_std::prim::PrimitiveCtor<T>>())};
  }

#define YAVE_DEF_PRIM_NODE(TYPE)                       \
  template struct node_declaration_traits<node::TYPE>; \
  template struct node_definition_traits<node::TYPE, modules::_std::tag>

  YAVE_DEF_PRIM_NODE(Num::Int);
  YAVE_DEF_PRIM_NODE(Num::Float);
  YAVE_DEF_PRIM_NODE(Logic::Bool);
  YAVE_DEF_PRIM_NODE(String::String);

} // namespace yave