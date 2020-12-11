//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/list/list.hpp>
#include <yave/signal/function.hpp>
#include <yave/rts/list.hpp>
#include <yave/obj/primitive/property.hpp>
#include <yave/node/core/structured_node_graph.hpp>

#include <range/v3/view.hpp>
#include <range/v3/action.hpp>
#include <range/v3/numeric.hpp>

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
      {"x", "xs"},
      {"x:xs"});
  }

  auto node_declaration_traits<node::List::Head>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "List.Head",
      "Get head of list",
      node_declaration_visibility::_public,
      {"x:xs"},
      {"x"});
  }

  auto node_declaration_traits<node::List::Tail>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "List.Tail",
      "Get head of list",
      node_declaration_visibility::_public,
      {"x:xs"},
      {"xs"});
  }

  auto node_declaration_traits<node::List::At>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "List.At",
      "Access list element by index",
      node_declaration_visibility::_public,
      {"[a]", "idx"},
      {"a"},
      {{1, make_node_argument<Int>(0)}});
  }

  auto node_declaration_traits<node::List::List>::get_node_declaration()
    -> node_declaration
  {
    namespace rn = ranges;
    namespace rv = ranges::views;
    namespace ra = ranges::actions;

    auto expand = [](
                    structured_node_graph& ng,
                    const node_handle& n) noexcept -> node_handle {
      // List.Cons
      auto cons = ng.search_path(
        yave::get_node_declaration<node::List::Cons>().full_name())[0];
      // List.Nil
      auto nil = ng.search_path(
        yave::get_node_declaration<node::List::Nil>().full_name())[0];

      assert(cons && nil);

      auto g   = ng.get_parent_group(n);
      auto iss = ng.input_sockets(n);

      auto cells =
        iss
        // make cons cell
        | rv::transform([&](auto&& s) {
            // cons cell to return
            auto cell = ng.create_copy(g, cons);
            // set source for input sockets
            ng.set_source_id(ng.input_sockets(cell)[0], s.id());
            // connect input
            if (auto ics = ng.connections(s); !ics.empty()) {
              auto c    = ics[0];
              auto info = ng.get_info(c);
              ng.connect(info->src_socket(), ng.input_sockets(cell)[0]);
            }
            return cell;
          })
        // add nil to last
        | rn::to_vector //
        | ra::push_back(ng.create_copy(g, nil));

      // connect cells
      for (size_t i = 0; i < cells.size() - 1; ++i)
        ng.connect(
          ng.output_sockets(cells[i + 1])[0], ng.input_sockets(cells[i])[1]);

      // replace macro node with cons tree
      auto ret = cells[0];
      for (auto&& c : ng.output_connections(n)) {
        auto info = ng.get_info(c);
        ng.disconnect(c);
        ng.connect(ng.output_sockets(ret)[0], info->dst_socket());
      }
      ng.destroy(n);

      return ret;
    };

    return macro_node_declaration(
      "List.List",
      "(Macro) Make list from arguments",
      node_declaration_visibility::_public,
      {},
      {"[a]"},
      expand);
  }

  namespace modules::_std::list {

    class ListNil_X;
    class ListCons_X;
    class ListHead_X;
    class ListTail_X;
    class ListAt_X;

    struct ListNil : SignalFunction<ListNil, List<ListNil_X>>
    {
      return_type code() const
      {
        return make_list<VarValueProxy<ListNil_X>>();
      }
    };

    struct ListCons : Function<
                        ListCons,
                        signal<ListCons_X>,
                        signal<List<signal<ListCons_X>>>,
                        FrameDemand,
                        List<signal<ListCons_X>>>
    {
      return_type code() const
      {
        auto fd = eval_arg<2>();
        auto e  = eval_arg<0>();
        auto l  = eval(arg<1>() << fd);
        return make_object<List<signal<ListCons_X>>>(e, l);
      }
    };

    struct ListHead
      : SignalFunction<ListHead, List<signal<ListHead_X>>, ListHead_X>
    {
      auto code() const -> return_type
      {
        return eval_arg<0>()->head() << arg_demand();
      }
    };

    struct ListTail : SignalFunction<
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
      : SignalFunction<ListAt, List<signal<ListAt_X>>, Int, ListAt_X>
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
      info.full_name(), 0, make_object<yave::modules::_std::list::ListNil>())};
  }

  auto node_definition_traits<node::List::Cons, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::List::Cons>();
    return {node_definition(
      info.full_name(), 0, make_object<yave::modules::_std::list::ListCons>())};
  }

  auto node_definition_traits<node::List::Head, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::List::Head>();
    return {node_definition(
      info.full_name(), 0, make_object<yave::modules::_std::list::ListHead>())};
  }

  auto node_definition_traits<node::List::Tail, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::List::Tail>();
    return {node_definition(
      info.full_name(), 0, make_object<yave::modules::_std::list::ListTail>())};
  }

  auto node_definition_traits<node::List::At, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::List::At>();
    return {node_definition(
      info.full_name(), 0, make_object<yave::modules::_std::list::ListAt>())};
  }
} // namespace yave