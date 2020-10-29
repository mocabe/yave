//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/list/list.hpp>
#include <yave/node/core/function.hpp>
#include <yave/rts/list.hpp>
#include <yave/obj/primitive/property.hpp>

namespace yave {

  auto node_declaration_traits<node::List::Nil>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "List.Nil",
      "Constructs empty list. Result can be passed to any type of node which "
      "takes list as input.",
      node_declaration_visibility::_public,
      {},
      {"[]"});
  }

  auto node_declaration_traits<node::List::Cons>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "List.Cons",
      "Construct new list node",
      node_declaration_visibility::_public,
      {"head", "tail"},
      {"cons"});
  }

  auto node_declaration_traits<node::List::Head>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "List.Head",
      "Get head of list",
      node_declaration_visibility::_public,
      {"list"},
      {"head"});
  }

  auto node_declaration_traits<node::List::Tail>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "List.Tail",
      "Get head of list",
      node_declaration_visibility::_public,
      {"list"},
      {"head"});
  }

  auto node_declaration_traits<node::List::At>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "List.At",
      "Access list element by index",
      node_declaration_visibility::_public,
      {"list", "index"},
      {"value"},
      {{1, make_node_argument<Int>(0)}});
  }

  namespace modules::_std::list {

    class ListNil_X;
    class ListCons_X;
    class ListHead_X;
    class ListTail_X;
    class ListAt_X;

    struct ListNil : NodeFunction<ListNil, List<ListNil_X>>
    {
      return_type code() const
      {
        return make_list<VarValueProxy<ListNil_X>>();
      }
    };

    struct ListCons : Function<
                        ListCons,
                        node_closure<ListCons_X>,
                        node_closure<List<node_closure<ListCons_X>>>,
                        FrameDemand,
                        List<node_closure<ListCons_X>>>
    {
      return_type code() const
      {
        auto fd = eval_arg<2>();
        auto e  = eval_arg<0>();
        auto l  = eval(arg<1>() << fd);
        return make_object<List<node_closure<ListCons_X>>>(e, l);
      }
    };

    struct ListHead
      : NodeFunction<ListHead, List<node_closure<ListHead_X>>, ListHead_X>
    {
      auto code() const -> return_type
      {
        return eval_arg<0>()->head() << arg_demand();
      }
    };

    struct ListTail : NodeFunction<
                        ListTail,
                        List<forall<ListTail_X>>,
                        List<forall<ListTail_X>>>
    {
      return_type code() const
      {
        return eval_arg<0>()->tail();
      }
    };

    struct ListAt
      : NodeFunction<ListAt, List<node_closure<ListAt_X>>, Int, ListAt_X>
    {
      return_type code() const
      {
        auto l   = eval_arg<0>();
        auto idx = eval_arg<1>();

        for (auto i = 0; i < *idx; ++i)
          l = eval(l->tail());

        return l->head() << arg_demand();
      }
    };
  } // namespace modules::_std::list

  auto node_definition_traits<node::List::Nil, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::List::Nil>();
    return {node_definition(
      get_full_name(info),
      0,
      make_object<yave::modules::_std::list::ListNil>())};
  }

  auto node_definition_traits<node::List::Cons, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::List::Cons>();
    return {node_definition(
      get_full_name(info),
      0,
      make_object<yave::modules::_std::list::ListCons>())};
  }

  auto node_definition_traits<node::List::Head, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::List::Head>();
    return {node_definition(
      get_full_name(info),
      0,
      make_object<yave::modules::_std::list::ListHead>())};
  }

  auto node_definition_traits<node::List::Tail, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::List::Tail>();
    return {node_definition(
      get_full_name(info),
      0,
      make_object<yave::modules::_std::list::ListTail>())};
  }

  auto node_definition_traits<node::List::At, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::List::At>();
    return {node_definition(
      get_full_name(info),
      0,
      make_object<yave::modules::_std::list::ListAt>())};
  }
} // namespace yave