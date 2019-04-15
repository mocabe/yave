//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/meta_type.hpp>

#include <utility> // make_index_sequence, etc.

namespace yave {

  template <class... Ts>
  class meta_set
  {
  public:
    meta_set(const meta_set&) = default;
    meta_set& operator=(const meta_set&) = default;

    [[nodiscard]] constexpr size_t size() const
    {
      return sizeof...(Ts);
    }

  private:
    constexpr meta_set() {};
    friend struct meta_set_access;
  };

  struct meta_set_access
  {
    template <class... Ts>
    [[nodiscard]] static constexpr auto create(meta_tuple<Ts...>)
    {
      return meta_set<Ts...> {};
    }
  };

  template <class... Ts1, class... Ts2>
  [[nodiscard]] constexpr auto
    make_set_impl(meta_tuple<Ts1...> tuple, meta_tuple<Ts2...> rest)
  {
    if constexpr (empty(rest))
      return tuple;
    else {
      auto h = head(rest);
      auto t = tail(rest);
      if constexpr (contains(h, tuple))
        return make_set_impl(tuple, t);
      else
        return make_set_impl(append(h, tuple), t);
    }
  }

  template <class... Ts>
  [[nodiscard]] constexpr auto make_set(meta_tuple<Ts...> tuple)
  {
    return meta_set_access::create(make_set_impl(tuple_c<>, tuple));
  }

  template <class T>
  [[nodiscard]] constexpr auto make_set(meta_type<T>)
  {
    return make_set(tuple_c<T>);
  }

  template <class... Ts>
  [[nodiscard]] constexpr auto make_tuple(meta_set<Ts...>)
  {
    return tuple_c<Ts...>;
  }

  template <class... Ts>
  [[nodiscard]] static constexpr auto set_c = make_set(tuple_c<Ts...>);

  template <class... Ts1, class... Ts2>
  [[nodiscard]] constexpr auto
    operator==(meta_set<Ts1...> s1, meta_set<Ts2...> s2)
  {
    return equal(s1, s2);
  }

  template <class... Ts1, class... Ts2>
  [[nodiscard]] constexpr auto
    operator!=(meta_set<Ts1...> s1, meta_set<Ts2...> s2)
  {
    return std::bool_constant<!(s1 == s2)> {};
  }

  // ------------------------------------------
  // includes

  template <class T1, class T2, size_t... S>
  [[nodiscard]] constexpr auto
    set_includes_impl(T1 t1, T2 t2, std::index_sequence<S...>)
  {
    (void)t1;
    (void)t2;

    if constexpr ((contains(get<S>(t1), t2) && ...))
      return true_c;
    else
      return false_c;
  }

  template <class... Ts1, class... Ts2>
  [[nodiscard]] constexpr auto
    includes(meta_set<Ts1...> t1, meta_set<Ts2...> t2)
  {
    return set_includes_impl(
      make_tuple(t1), make_tuple(t2), std::make_index_sequence<t1.size()>());
  }

  // ------------------------------------------
  // equal

  template <class... Ts1, class... Ts2>
  [[nodiscard]] constexpr auto equal(meta_set<Ts1...> t1, meta_set<Ts2...> t2)
  {
    if constexpr (includes(t1, t2) && includes(t2, t1))
      return true_c;
    else
      return false_c;
  }

  // ------------------------------------------
  // insert

  template <class T, class... Ts>
  [[nodiscard]] constexpr auto insert(meta_type<T> t, meta_set<Ts...> s)
  {
    if constexpr (includes(make_set(t), s))
      return s;
    else
      return set_c<Ts..., T>;
  }

} // namespace yave