//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/list/algorithm.hpp>
#include <yave/rts/list.hpp>
#include <yave/signal/function.hpp>
#include <yave/signal/list.hpp>
#include <yave/obj/primitive/property.hpp>

namespace yave {

  auto node_declaration_traits<node::List::Algo::Map>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "List.Algo.Map",
      "Apply transform to all list elements.\n"
      "\n"
      "param:\n"
      "  list: List of type a\n"
      "  fn  : Mapping function of (a->b)\n"
      "\n"
      "return:\n"
      "  List of type b\n",
      node_declaration_visibility::_public,
      {"list", "fn"},
      {"list"});
  }

  auto node_declaration_traits<node::List::Algo::Repeat>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "List.Algo.Repeat",
      "Repeat elements in list\n"
      "\n"
      "param:\n"
      "  a  : Value of type a\n"
      "  num: Number of copies to create\n"
      "\n"
      "return:\n"
      "  List of type a\n",
      node_declaration_visibility::_public,
      {"a", "num"},
      {"list"},
      {{1, make_node_argument<Int>(1)}});
  }

  auto node_declaration_traits<
    node::List::Algo::Enumerate>::get_node_declaration() -> node_declaration
  {
    return function_node_declaration(
      "List.Algo.Enumerate",
      "Traverse list elements with index argument\n"
      "\n"
      "param:\n"
      "  list: List of type a\n"
      "  fn  : Mapping function of (Int -> a -> b)\n"
      "\n"
      "return:\n"
      "  List of type b\n",
      node_declaration_visibility::_public,
      {"list", "fn"},
      {"list"});
  }

  auto node_declaration_traits<node::List::Algo::Fold>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "List.Algo.Fold",
      "Left fold list\n"
      "\n"
      "param:\n"
      "  list: List of type a\n"
      "  fn  : Fold function of (b -> a -> b)\n"
      "\n"
      "return:\n"
      "  Folded value of type b\n",
      node_declaration_visibility::_public,
      {"list", "fn", "init"},
      {"b"});
  }

  namespace modules::_std::list {

    class X;
    class Y;

    // (FD->[FD->X]) -> ((FD->X)->(FD->Y)) -> (FD->[FD->Y])
    struct LazyListMap
      : SignalFunction<LazyListMap, SList<X>, sf<X, Y>, SList<Y>>
    {
      auto code() const -> return_type
      {
        // [FD->X]
        auto l = eval_arg<0>();
        // (FD->X) -> (FD->Y)
        auto f = arg_signal<1>();

        if (l->is_nil())
          return make_object<SList<Y>>();

        struct ListMapH : Function<ListMapH, SList<X>, FrameDemand, SList<X>>
        {
          auto code() const -> return_type
          {
            return arg<0>();
          }
        };

        return make_object<SList<Y>>(
          f << l->head(),
          make_object<LazyListMap>()
            << (make_object<ListMapH>() << l->tail()) << f << arg_demand());
      }
    };

    struct StrictListMap
      : SignalFunction<StrictListMap, SList<X>, sf<X, Y>, SList<Y>>
    {
      auto code() const -> return_type
      {
        auto l = eval_arg<0>();
        auto f = arg_signal<1>();

        auto ret = make_object<SList<Y>>();

        while (!l->is_nil()) {
          ret = make_object<SList<Y>>(f << l->head(), ret);
          l   = eval(l->tail());
        }
        return ret;
      }
    };

    struct ListRepeat : SignalFunction<ListRepeat, X, Int, SList<X>>
    {
      auto code() const -> return_type
      {
        auto e = arg_signal<0>();
        auto n = eval_arg<1>();

        auto ret = make_object<SList<X>>();

        if (*n < 0)
          return ret;

        for (auto i = 0; i < *n; ++i)
          ret = make_object<SList<X>>(e, ret);

        return ret;
      }
    };

    struct ListEnumerate
      : SignalFunction<ListEnumerate, SList<X>, sf<Int, X, Y>, SList<Y>>
    {
      auto code() const -> return_type
      {
        auto l = eval_arg<0>();
        auto f = arg_signal<1>();

        auto ret = make_object<SList<Y>>();

        // \fd.idx
        struct ListEnumerateH : SignalFunction<ListEnumerateH, Int>
        {
          int m_idx;

          ListEnumerateH(int idx)
            : m_idx {idx}
          {
          }

          auto code() const -> return_type
          {
            return make_object<Int>(m_idx);
          }
        };

        auto idx = 0;
        while (!l->is_nil()) {
          ret = make_object<SList<Y>>(
            eval(f << make_object<ListEnumerateH>(idx) << l->head()), ret);
          l = eval(l->tail());
          ++idx;
        }
        return ret;
      }
    };

    struct ListFold : SignalFunction<ListFold, SList<X>, sf<Y, X, Y>, Y, Y>
    {
      auto code() const -> return_type
      {
        auto l = eval_arg<0>();
        auto f = arg_signal<1>();
        auto v = eval(arg_signal<2>());

        while (!l->is_nil()) {
          v = eval(f << v << l->head());
          l = eval(l->tail());
        }
        return v << arg_demand();
      }
    };

  } // namespace modules::_std::list

  auto node_definition_traits<node::List::Algo::Map, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::List::Algo::Map>();
    return {node_definition(
      info.full_name(),
      0,
      make_object<yave::modules::_std::list::StrictListMap>())};
  }

  auto node_definition_traits<node::List::Algo::Repeat, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::List::Algo::Repeat>();
    return {node_definition(
      info.full_name(),
      0,
      make_object<yave::modules::_std::list::ListRepeat>())};
  }

  auto node_definition_traits<node::List::Algo::Enumerate, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::List::Algo::Enumerate>();
    return {node_definition(
      info.full_name(),
      0,
      make_object<yave::modules::_std::list::ListEnumerate>())};
  }

  auto node_definition_traits<node::List::Algo::Fold, modules::_std::tag>::
    get_node_definitions() -> std::vector<node_definition>
  {
    auto info = get_node_declaration<node::List::Algo::Fold>();
    return {node_definition(
      info.full_name(), 0, make_object<yave::modules::_std::list::ListFold>())};
  }
} // namespace yave