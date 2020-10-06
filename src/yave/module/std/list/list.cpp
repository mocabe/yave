//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/list/list.hpp>
#include <yave/node/core/function.hpp>
#include <yave/rts/list.hpp>

namespace yave {

  auto node_declaration_traits<node::ListNil>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Std.List.Nil",
      "Constructs empty list. Result can be passed to any type of node which "
      "takes list as input.",
      {},
      {"[]"});
  }

  auto node_declaration_traits<node::ListCons>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Std.List.Cons", "Construct new list node", {"head", "tail"}, {"cons"});
  }

  auto node_declaration_traits<node::ListDecompose>::get_node_declaration()
    -> node_declaration
  {
    return node_declaration(
      "Std.List.Match",
      "Decompose list into head and tail",
      {"list"},
      {"head", "tail"});
  }

  namespace modules::_std::list {

    class ListNil_X;
    class ListCons_X;
    class ListHead_X;
    class ListTail_X;

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

    struct ListHead : Function<
                        ListHead,
                        node_closure<List<node_closure<ListHead_X>>>,
                        FrameDemand,
                        ListHead_X>
    {
      auto code() const -> return_type
      {
        return eval(arg<0>() << arg<1>())->head() << arg<1>();
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
  } // namespace modules::_std::list

  auto node_definition_traits<node::ListNil, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::ListNil>();
    return {node_definition(
      info.full_name(), 0, make_object<yave::modules::_std::list::ListNil>())};
  }

  auto node_definition_traits<node::ListCons, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::ListCons>();
    return {node_definition(
      info.full_name(), 0, make_object<yave::modules::_std::list::ListCons>())};
  }

  auto node_definition_traits<node::ListDecompose, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::ListDecompose>();

    auto d1 = node_definition(
      info.full_name(), 0, make_object<yave::modules::_std::list::ListHead>());

    auto d2 = node_definition(
      info.full_name(), 1, make_object<yave::modules::_std::list::ListTail>());

    return {std::move(d1), std::move(d2)};
  }
} // namespace yave