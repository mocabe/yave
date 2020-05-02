//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/meta_type.hpp>

namespace yave {

  /// meta_tuple
  template <class... Ts>
  struct meta_tuple
  {
    [[nodiscard]] constexpr size_t size() const
    {
      return sizeof...(Ts);
    }
  };

  /// meta_tuple constant
  template <class... Ts>
  inline constexpr meta_tuple<Ts...> tuple_c {};

  template <class... Ts1, class... Ts2>
  [[nodiscard]] constexpr auto equal(
    meta_tuple<Ts1...> t1,
    meta_tuple<Ts2...> t2)
  {
    if constexpr (std::is_same_v<decltype(t1), decltype(t2)>)
      return true_c;
    else
      return false_c;
  }

  /// operator==
  template <class... Ts1, class... Ts2>
  [[nodiscard]] constexpr auto operator==(
    meta_tuple<Ts1...> t1,
    meta_tuple<Ts2...> t2)
  {
    return equal(t1, t2);
  }

  /// operaotr!=
  template <class... Ts1, class... Ts2>
  [[nodiscard]] constexpr auto operator!=(
    meta_tuple<Ts1...> t1,
    meta_tuple<Ts2...> t2)
  {
    return std::bool_constant<!(t1 == t2)> {};
  }

  // ------------------------------------------
  // is_tuple

  template <class... Ts>
  [[nodiscard]] constexpr auto is_tuple(meta_tuple<Ts...>)
  {
    return true_c;
  }

  template <class T>
  [[nodiscard]] constexpr auto is_tuple(T)
  {
    return false_c;
  }

  // ------------------------------------------
  // make

  /// Make meta tuple from types.
  template <class... Ts>
  [[nodiscard]] constexpr auto make_tuple(meta_type<Ts>...)
  {
    return tuple_c<Ts...>;
  }

  // ------------------------------------------
  // get

  namespace detail {

    template <size_t It, size_t Idx, class H, class... Ts>
    constexpr auto tuple_get_impl(meta_tuple<H, Ts...>)
    {
      if constexpr (It == Idx)
        return type_c<H>;
      else
        return tuple_get_impl<It + 1, Idx>(tuple_c<Ts...>);
    }

  } // namespace detail

  /// Get element of tuple.
  template <size_t Idx, class... Ts>
  [[nodiscard]] constexpr auto get(meta_tuple<Ts...> tuple)
  {
    static_assert(Idx < tuple.size(), "Index out of range");
    return detail::tuple_get_impl<0, Idx>(tuple);
  }

  // ------------------------------------------
  // head

  /// Get head of tuple.
  template <class H, class... Ts>
  [[nodiscard]] constexpr auto head(meta_tuple<H, Ts...>)
  {
    return type_c<H>;
  }

  // ------------------------------------------
  // tail

  /// Get tail of tuple.
  template <class H, class... Ts>
  [[nodiscard]] constexpr auto tail(meta_tuple<H, Ts...>)
  {
    return tuple_c<Ts...>;
  }

  [[nodiscard]] constexpr auto tail(meta_tuple<>)
  {
    return tuple_c<>;
  }

  // ------------------------------------------
  // last

  /// Get last element of tuple.
  template <class... Ts>
  [[nodiscard]] constexpr auto last(meta_tuple<Ts...> t)
  {
    if constexpr (empty(t))
      static_assert(false_v<Ts...>, "Empty tuple");

    if constexpr (t.size() <= 1)
      return head(t);
    else
      return last(tail(t));
  }

  // ------------------------------------------
  // empty

  /// empty
  template <class... Ts>
  [[nodiscard]] constexpr auto empty(meta_tuple<Ts...>)
  {
    if constexpr (sizeof...(Ts) == 0)
      return true_c;
    else
      return false_c;
  }

  // ------------------------------------------
  // append

  /// Append tuple.
  template <class E, class... Ts>
  [[nodiscard]] constexpr auto append(meta_type<E>, meta_tuple<Ts...>)
  {
    return tuple_c<Ts..., E>;
  }

  /// Append tuple.
  template <class E>
  [[nodiscard]] constexpr auto append(meta_type<E>, meta_tuple<>)
  {
    return tuple_c<E>;
  }

  // ------------------------------------------
  // concat

  /// Concat tuple.
  template <class... Ts1, class... Ts2>
  [[nodiscard]] constexpr auto concat(meta_tuple<Ts1...>, meta_tuple<Ts2...>)
  {
    return tuple_c<Ts1..., Ts2...>;
  }

  // ------------------------------------------
  // intersect

  namespace detail {

    template <class... Ts1, class... Rs>
    constexpr auto intersect_impl(
      meta_tuple<Ts1...>,
      meta_tuple<>,
      meta_tuple<Rs...> result)
    {
      return result;
    }

    template <class... Ts1, class... Ts2, class... Rs>
    constexpr auto intersect_impl(
      meta_tuple<Ts1...> ts1,
      meta_tuple<Ts2...> ts2,
      meta_tuple<Rs...> result)
    {
      if constexpr (contains(head(ts2), ts1))
        return intersect_impl(ts1, tail(ts2), append(head(ts2), result));
      else
        return intersect_impl(ts1, tail(ts2), result);
    }
  } // namespace detail

  template <class... Ts1, class... Ts2>
  [[nodiscard]] constexpr auto intersect(
    meta_tuple<Ts1...> ts1,
    meta_tuple<Ts2...> ts2)
  {
    return detail::intersect_impl(ts1, ts2, tuple_c<>);
  }

  // ------------------------------------------
  // flatten

  namespace detail {
    template <class... Tups, class... Ts>
    constexpr auto flatten_impl(
      meta_tuple<Tups...> tuples,
      meta_tuple<Ts...> result)
    {
      if constexpr (empty(tuples))
        return result;
      else {
        return flatten_impl(
          tail(tuples),
          // head(tuples) is type_c<meta_tuple<...>>
          concat(result, typename decltype(head(tuples))::type {}));
      }
    }
  } // namespace detail

  /// Flatten tuple of tuples
  template <class... Tups>
  [[nodiscard]] constexpr auto flatten(meta_tuple<Tups...> tuples)
  {
    if constexpr ((is_tuple(Tups {}) && ...))
      return detail::flatten_impl(tuples, tuple_c<>);
    else
      static_assert(false_v<Tups...>, "not tuple of tuples");
  }

  // ------------------------------------------
  // contains

  /// Check if the tuple contains specific type.
  template <class E, class... Ts>
  [[nodiscard]] constexpr auto contains(meta_type<E>, meta_tuple<Ts...>)
  {
    constexpr bool b = ((type_c<Ts> == type_c<E>) || ...);
    return std::bool_constant<b> {};
  }

  // ------------------------------------------
  // remove_last

  /// Remove last element from tuple.
  template <class... Ts>
  [[nodiscard]] constexpr auto remove_last(meta_tuple<Ts...> tuple)
  {
    if constexpr (tuple.size() <= 1)
      return tuple_c<>;
    else
      return concat(make_tuple(head(tuple)), remove_last(tail(tuple)));
  }

  // ------------------------------------------
  // map

  namespace detail {

    template <class F, class... Rs>
    constexpr auto map_impl(meta_tuple<>, F, meta_tuple<Rs...> result)
    {
      return result;
    }

    template <class... Ts, class F, class... Rs>
    constexpr auto map_impl(
      meta_tuple<Ts...> tuple,
      F f,
      meta_tuple<Rs...> result)
    {
      return map_impl(tail(tuple), f, append(f(head(tuple)), result));
    }

  } // namespace detail

  template <class... Ts, class F>
  [[nodiscard]] constexpr auto map(meta_tuple<Ts...> tuple, F f)
  {
    return detail::map_impl(tuple, f, tuple_c<>);
  }

  // ------------------------------------------
  // all

  template <class F>
  [[nodiscard]] constexpr auto all(meta_tuple<>, F)
  {
    return true_c;
  }

  template <class... Ts, class F>
  [[nodiscard]] constexpr auto all(meta_tuple<Ts...>, F f)
  {
    constexpr bool b = (f(type_c<Ts>) && ...);
    return std::bool_constant<b> {};
  }

} // namespace yave