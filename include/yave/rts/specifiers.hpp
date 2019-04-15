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

  // ------------------------------------------
  // has_specifier

  namespace detail {

    template <class T, class = void>
    struct has_specifier_impl
    {
      static constexpr auto value = false_c;
    };

    template <class T>
    struct has_specifier_impl<T, std::void_t<decltype(T::specifier)>>
    {
      static constexpr auto value = true_c;
    };

  } // namespace detail

  template <class T>
  constexpr auto has_specifier()
  {
    return detail::has_specifier_impl<T>::value;
  }

  // ------------------------------------------
  // get_specifier

  template <class T>
  constexpr auto get_specifier(meta_type<T> = {})
  {
    if constexpr (has_specifier<T>())
      return T::specifier;
    else
      // lift
      return type_c<object<T>>;
  }

  // ------------------------------------------
  // is_specifier

  template <class T>
  constexpr auto is_specifier(meta_type<T>)
  {
    return false_c;
  }

  template <class... Ts>
  constexpr auto is_specifier(meta_type<closure<Ts...>>)
  {
    return true_c;
  }

  template <class Tag>
  constexpr auto is_specifier(meta_type<forall<Tag>>)
  {
    return true_c;
  }

  template <class Tag>
  constexpr auto is_specifier(meta_type<object<Tag>>)
  {
    return true_c;
  }

  // ------------------------------------------
  // normalize_specifier

  /// lift all raw types to specifiers
  template <class T>
  constexpr auto normalize_specifier(meta_type<T> t)
  {
    if constexpr (is_specifier(t))
      return t;
    else if constexpr (!is_complete_v<T>)
      // type variable
      return type_c<forall<T>>;
    else
      // object
      return get_specifier(t);
  }

  template <class... Ts>
  constexpr auto normalize_specifier(meta_type<closure<Ts...>>)
  {
    return type_c<
      closure<typename decltype(normalize_specifier(type_c<Ts>))::type...>>;
  }

  // ------------------------------------------
  // get_proxy_type

  template <class... Ts>
  constexpr auto get_proxy_type(meta_type<closure<Ts...>>)
  {
    return type_c<
      ClosureProxy<typename decltype(get_proxy_type(type_c<Ts>))::type...>>;
  }

  template <class Tag>
  constexpr auto get_proxy_type(meta_type<forall<Tag>>)
  {
    return type_c<VarValueProxy<Tag>>;
  }

  template <class T>
  constexpr auto get_proxy_type(meta_type<object<T>>)
  {
    return type_c<ObjectProxy<T>>;
  }

  // ------------------------------------------
  // get_argument_proxy_type

  template <class... Ts>
  constexpr auto get_argument_proxy_type(meta_type<closure<Ts...>>)
  {
    return type_c<ClosureArgumentProxy<typename decltype(
      get_argument_proxy_type(type_c<Ts>))::type...>>;
  }

  template <class Tag>
  constexpr auto get_argument_proxy_type(meta_type<forall<Tag>> v)
  {
    return get_proxy_type(v);
  }

  template <class T>
  constexpr auto get_argument_proxy_type(meta_type<object<T>> o)
  {
    return get_proxy_type(o);
  }

  // ------------------------------------------
  // get_object_type

  template <class T>
  constexpr auto get_object_type(meta_type<object<T>>)
  {
    return type_c<T>;
  }

  template <class T>
  constexpr auto get_object_type(meta_type<ObjectProxy<T>>)
  {
    return type_c<T>;
  }

  // ------------------------------------------
  // utility

  /// append to ClosureProxy
  template <class... Ts, class T>
  constexpr auto append(meta_type<T>, meta_type<ClosureProxy<Ts...>>)
  {
    return type_c<ClosureProxy<Ts..., T>>;
  }

} // namespace yave
