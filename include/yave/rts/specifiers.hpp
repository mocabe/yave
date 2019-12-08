//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/meta_type.hpp>
#include <yave/rts/meta_tuple.hpp>

namespace yave {

  /// closure specifier
  template <class... Ts>
  struct closure;

  /// forall specifier
  template <class Tag>
  struct forall;

  /// object specifier
  template <class T>
  struct object;

  /// list specifier
  template <class T>
  struct list;

  // ------------------------------------------
  // proxy types (forward decl)

  template <class... Ts>
  struct ClosureProxy;

  template <class... Ts>
  struct ClosureArgumentProxy;

  template <class Tag>
  struct VarValueProxy;

  template <class T>
  struct ObjectProxy;

  template <class T>
  struct ListProxy;

  // ------------------------------------------
  // is_specifier

  template <class T>
  [[nodiscard]] constexpr auto is_specifier(meta_type<T>)
  {
    return false_c;
  }

  template <class... Ts>
  [[nodiscard]] constexpr auto is_specifier(meta_type<closure<Ts...>>)
  {
    return true_c;
  }

  template <class Tag>
  [[nodiscard]] constexpr auto is_specifier(meta_type<forall<Tag>>)
  {
    return true_c;
  }

  template <class Tag>
  [[nodiscard]] constexpr auto is_specifier(meta_type<object<Tag>>)
  {
    return true_c;
  }

  template <class T>
  [[nodiscard]] constexpr auto is_specifier(meta_type<list<T>>)
  {
    return true_c;
  }

  // ------------------------------------------
  // normalize_specifier

  struct Object;
  struct Undefined;

  /// lift all raw types to specifiers
  template <class T>
  [[nodiscard]] constexpr auto normalize_specifier(meta_type<T> t)
  {
    if constexpr (is_specifier(t))
      return t;
    else if constexpr (!is_complete_v<T>)
      // type variable
      return type_c<forall<T>>;
    else if constexpr (std::is_base_of_v<Object, T>)
      // object
      return type_c<object<T>>;
    else
      static_assert(false_v<T>, "T is not valid heap object");
  }

  [[nodiscard]] constexpr auto normalize_specifier(meta_type<Undefined>)
  {
    return type_c<object<Undefined>>;
  }

  template <class... Ts>
  [[nodiscard]] constexpr auto normalize_specifier(meta_type<closure<Ts...>>)
  {
    return type_c<
      closure<typename decltype(normalize_specifier(type_c<Ts>))::type...>>;
  }

  template <class T>
  [[nodiscard]] constexpr auto normalize_specifier(meta_type<list<T>>)
  {
    return type_c<
      list<typename decltype(normalize_specifier(type_c<T>))::type>>;
  }

  // ------------------------------------------
  // get_proxy_type

  template <class... Ts>
  [[nodiscard]] constexpr auto get_proxy_type(meta_type<closure<Ts...>>)
  {
    return type_c<
      ClosureProxy<typename decltype(get_proxy_type(type_c<Ts>))::type...>>;
  }

  template <class Tag>
  [[nodiscard]] constexpr auto get_proxy_type(meta_type<forall<Tag>>)
  {
    return type_c<VarValueProxy<Tag>>;
  }

  template <class T>
  [[nodiscard]] constexpr auto get_proxy_type(meta_type<object<T>>)
  {
    return type_c<ObjectProxy<T>>;
  }

  template <class T>
  [[nodiscard]] constexpr auto get_proxy_type(meta_type<list<T>>)
  {
    return type_c<
      ListProxy<typename decltype(get_proxy_type(type_c<T>))::type>>;
  }

  // ------------------------------------------
  // get_argument_proxy_type

  template <class... Ts>
  [[nodiscard]] constexpr auto get_argument_proxy_type(
    meta_type<closure<Ts...>>)
  {
    return type_c<ClosureArgumentProxy<typename decltype(
      get_argument_proxy_type(type_c<Ts>))::type...>>;
  }

  template <class Tag>
  [[nodiscard]] constexpr auto get_argument_proxy_type(meta_type<forall<Tag>> v)
  {
    return get_proxy_type(v);
  }

  template <class T>
  [[nodiscard]] constexpr auto get_argument_proxy_type(meta_type<object<T>> o)
  {
    return get_proxy_type(o);
  }

  template <class T>
  [[nodiscard]] constexpr auto get_argument_proxy_type(meta_type<list<T>> l)
  {
    return get_proxy_type(l);
  }

  // ------------------------------------------
  // get_value_object_type

  template <class T>
  struct ty_value;

  template <class T>
  [[nodiscard]] constexpr auto get_value_object_type(meta_type<ty_value<T>>)
  {
    return type_c<T>;
  }

  // ------------------------------------------
  // get_list_object_type

  template <class T>
  struct Box;

  template <class T>
  struct list_object_value;

  template <class T>
  using List = Box<list_object_value<T>>;

  template <class T>
  struct ty_list;

  template <class T>
  [[nodiscard]] constexpr auto get_list_object_type(meta_type<ty_list<T>>)
  {
    return type_c<List<T>>;
  }

  // ------------------------------------------
  // utility

  /// append to ClosureProxy
  template <class... Ts, class T>
  [[nodiscard]] constexpr auto append(
    meta_type<T>,
    meta_type<ClosureProxy<Ts...>>)
  {
    return type_c<ClosureProxy<Ts..., T>>;
  }

} // namespace yave
