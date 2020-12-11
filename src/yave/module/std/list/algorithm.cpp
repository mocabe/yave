//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/list/algorithm.hpp>
#include <yave/rts/list.hpp>
#include <yave/signal/function.hpp>
#include <yave/obj/primitive/property.hpp>

namespace yave {

  auto node_declaration_traits<node::List::Algo::Map>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "List.Algo.Map",
      "[a]->(a->b)->[b]\n"
      "Apply transform to list elements",
      node_declaration_visibility::_public,
      {"[a]", "a->b"},
      {"[b]"});
  }

  auto node_declaration_traits<node::List::Algo::Repeat>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "List.Algo.Repeat",
      "a->Int->[a]\n"
      "Repeat elements in list",
      node_declaration_visibility::_public,
      {"a", "Int"},
      {"[a]"},
      {{1, make_node_argument<Int>(1)}});
  }

  auto node_declaration_traits<
    node::List::Algo::Enumerate>::get_node_declaration() -> node_declaration
  {
    return function_node_declaration(
      "List.Algo.Enumerate",
      "[a]->(Int->a->b)->[b]\n"
      "Enumerate list elements",
      node_declaration_visibility::_public,
      {"[a]", "Int->a->b"},
      {"[b]"});
  }

  auto node_declaration_traits<node::List::Algo::Fold>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "List.Algo.Fold",
      "[a]->(b->a->b)->b->b\n"
      "Left fold list",
      node_declaration_visibility::_public,
      {"[a]", "b->a->b", "b"},
      {"b"});
  }

  namespace modules::_std::list {

    class X;
    class Y;

    // (FD->[FD->X]) -> ((FD->X)->(FD->Y)) -> (FD->[FD->Y])
    struct LazyListMap : Function<
                           LazyListMap,
                           signal<List<signal<X>>>,
                           signal<X, Y>,
                           FrameDemand,
                           List<signal<Y>>>
    {
      auto code() const -> return_type
      {
        auto fd = eval_arg<2>();
        // [FD->X]
        auto l = eval(arg<0>() << fd);
        // (FD->X) -> (FD->Y)
        auto f = eval_arg<1>();

        if (l->is_nil())
          return make_object<List<signal<Y>>>();

        struct ListMapH
          : Function<ListMapH, List<signal<X>>, FrameDemand, List<signal<X>>>
        {
          auto code() const -> return_type
          {
            return arg<0>();
          }
        };

        return make_object<List<signal<Y>>>(
          f << l->head(),
          make_object<LazyListMap>()
            << (make_object<ListMapH>() << l->tail()) << f << fd);
      }
    };

    struct StrictListMap : Function<
                             StrictListMap,
                             signal<List<signal<X>>>,
                             signal<X, Y>,
                             FrameDemand,
                             List<signal<Y>>>
    {
      auto code() const -> return_type
      {
        auto fd = eval_arg<2>();
        auto l  = eval(arg<0>() << fd);
        auto f  = eval_arg<1>();

        auto ret = make_object<List<signal<Y>>>();

        while (!l->is_nil()) {
          ret = make_object<List<signal<Y>>>(eval(f << l->head()), ret);
          l   = eval(l->tail());
        }

        return ret;
      }
    };

    struct ListRepeat : Function<
                          ListRepeat,
                          signal<X>,
                          signal<Int>,
                          FrameDemand,
                          List<signal<X>>>
    {
      auto code() const -> return_type
      {
        auto fd = eval_arg<2>();
        auto e  = eval_arg<0>();
        auto n  = eval(arg<1>() << fd);

        auto ret = make_object<List<signal<X>>>();

        if (*n < 0)
          return ret;

        for (auto i = 0; i < *n; ++i)
          ret = make_object<List<signal<X>>>(e, ret);

        return ret;
      }
    };

    struct ListEnumerate : Function<
                             ListEnumerate,
                             signal<List<signal<X>>>,
                             signal<Int, X, Y>,
                             FrameDemand,
                             List<signal<Y>>>
    {
      auto code() const -> return_type
      {
        auto fd = eval_arg<2>();
        auto l  = eval(arg<0>() << fd);
        auto f  = eval_arg<1>();

        auto ret = make_object<List<signal<Y>>>();

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
          ret = make_object<List<signal<Y>>>(
            eval(f << make_object<ListEnumerateH>(idx) << l->head()), ret);
          l = eval(l->tail());
          ++idx;
        }

        return ret;
      }
    };

    struct ListFold : Function<
                        ListFold,
                        signal<List<signal<X>>>,
                        signal<Y, X, Y>,
                        signal<Y>,
                        FrameDemand,
                        Y>
    {
      auto code() const -> return_type
      {
        auto fd = eval_arg<3>();
        auto l  = eval(arg<0>() << fd);
        auto f  = eval_arg<1>();
        auto v  = eval_arg<2>();

        while (!l->is_nil()) {
          v = eval(f << v << l->head());
          l = eval(l->tail());
        }

        return v << fd;
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