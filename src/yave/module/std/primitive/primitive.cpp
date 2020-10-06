//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/primitive/primitive.hpp>
#include <yave/module/std/num/num.hpp>
#include <yave/module/std/string/string.hpp>
#include <yave/module/std/bool/bool.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/primitive/property.hpp>

namespace yave {

  template <class T>
  auto node_declaration_traits<node::PrimitiveCtor<T>>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      std::string(module) + "." + node_name,
      "Data type constructor",
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

  YAVE_DEF_PRIM_NODE(Int);
  YAVE_DEF_PRIM_NODE(Float);
  YAVE_DEF_PRIM_NODE(Bool);
  YAVE_DEF_PRIM_NODE(String);

} // namespace yave