//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#include <yave/module/std/list/algorithm.hpp>
#include <yave/rts/list.hpp>
#include <yave/node/core/function.hpp>
#include <yave/obj/primitive/property.hpp>

namespace yave {

  auto node_declaration_traits<node::List::Algo::Map>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "List.Algo.Map",
      "[a] -> (a -> b) -> [b]\n"
      "Apply transform to list elements",
      node_declaration_visibility::_public,
      {"[a]", "a -> b"},
      {"[b]"});
  }

  auto node_declaration_traits<node::List::Algo::Repeat>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "List.Algo.Repeat",
      "a -> Int -> [a]\n"
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
      "[a] -> (Int -> a -> b) -> [b]\n"
      "Enumerate list elements",
      node_declaration_visibility::_public,
      {"[a]", "Int -> a -> b"},
      {"[b]"});
  }

  auto node_declaration_traits<node::List::Algo::Fold>::get_node_declaration()
    -> node_declaration
  {
    return function_node_declaration(
      "List.Algo.Fold",
      "[a] -> (b -> a -> b) -> b -> b\n"
      "Left fold list",
      node_declaration_visibility::_public,
      {"[a]", "b -> a -> b", "b"},
      {"b"});
  }

  namespace modules::_std::list {

    class ListMap_X;
    class ListMap_Y;

    // (FD->[FD->X]) -> ((FD->X)->(FD->Y)) -> (FD->[FD->Y])
    struct LazyListMap : Function<
                           LazyListMap,
                           node_closure<List<node_closure<ListMap_X>>>,
                           node_closure<ListMap_X, ListMap_Y>,
                           FrameDemand,
                           List<node_closure<ListMap_Y>>>
    {
      auto code() const -> return_type
      {
        auto fd = eval_arg<2>();
        // [FD->X]
        auto l = eval(arg<0>() << fd);
        // (FD->X) -> (FD->Y)
        auto f = eval_arg<1>();

        if (l->is_nil())
          return make_object<List<node_closure<ListMap_Y>>>();

        struct ListMapH : Function<
                            ListMapH,
                            List<node_closure<ListMap_X>>,
                            FrameDemand,
                            List<node_closure<ListMap_X>>>
        {
          auto code() const -> return_type
          {
            return arg<0>();
          }
        };

        return make_object<List<node_closure<ListMap_Y>>>(
          f << l->head(),
          make_object<LazyListMap>()
            << (make_object<ListMapH>() << l->tail()) << f << fd);
      }
    };

    struct StrictListMap : Function<
                             StrictListMap,
                             node_closure<List<node_closure<ListMap_X>>>,
                             node_closure<ListMap_X, ListMap_Y>,
                             FrameDemand,
                             List<node_closure<ListMap_Y>>>
    {
      auto code() const -> return_type
      {
        auto fd = eval_arg<2>();
        auto l  = eval(arg<0>() << fd);
        auto f  = eval_arg<1>();

        auto ret = make_object<List<node_closure<ListMap_Y>>>();

        while (!l->is_nil()) {
          ret = make_object<List<node_closure<ListMap_Y>>>(
            eval(f << l->head()), ret);
          l = eval(l->tail());
        }

        return ret;
      }
    };

    class ListRepeat_X;

    struct ListRepeat : Function<
                          ListRepeat,
                          node_closure<ListRepeat_X>,
                          node_closure<Int>,
                          FrameDemand,
                          List<node_closure<ListRepeat_X>>>
    {
      auto code() const -> return_type
      {
        auto fd = eval_arg<2>();
        auto e  = eval_arg<0>();
        auto n  = eval(arg<1>() << fd);

        auto ret = make_object<List<node_closure<ListRepeat_X>>>();

        if (*n < 0)
          return ret;

        for (auto i = 0; i < *n; ++i)
          ret = make_object<List<node_closure<ListRepeat_X>>>(e, ret);

        return ret;
      }
    };

    class ListEnum_X;
    class ListEnum_Y;

    struct ListEnumerate : Function<
                             ListEnumerate,
                             node_closure<List<node_closure<ListEnum_X>>>,
                             node_closure<Int, ListEnum_X, ListEnum_Y>,
                             FrameDemand,
                             List<node_closure<ListEnum_Y>>>
    {
      auto code() const -> return_type
      {
        auto fd = eval_arg<2>();
        auto l  = eval(arg<0>() << fd);
        auto f  = eval_arg<1>();

        auto ret = make_object<List<node_closure<ListEnum_Y>>>();

        // \fd.idx
        struct ListEnumerateH : NodeFunction<ListEnumerateH, Int>
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
          ret = make_object<List<node_closure<ListEnum_Y>>>(
            eval(f << make_object<ListEnumerateH>(idx) << l->head()), ret);
          l = eval(l->tail());
          ++idx;
        }

        return ret;
      }
    };

    class ListFold_X;
    class ListFold_Y;

    struct ListFold : Function<
                        ListFold,
                        node_closure<List<node_closure<ListFold_X>>>,
                        node_closure<ListFold_Y, ListFold_X, ListFold_Y>,
                        node_closure<ListFold_Y>,
                        FrameDemand,
                        ListFold_Y>
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