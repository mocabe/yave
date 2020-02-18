
#pragma once

#include <type_traits>

namespace yave {

  // ------------------------------------------
  // false_v

  template <class...>
  struct make_false
  {
    static constexpr bool value = false;
  };

  template <class... Ts>
  static constexpr bool false_v = make_false<Ts...>::value;

  // ------------------------------------------
  // is_complete

  namespace detail {

    template <class T>
    constexpr char is_complete_impl_func(int (*)[sizeof(T)]);

    template <class>
    constexpr long is_complete_impl_func(...);

    template <class T>
    struct is_complete_impl
    {
      static constexpr bool value = sizeof(is_complete_impl_func<T>(0)) == 1;
    };

  } // namespace detail

  template <class T>
  constexpr bool is_complete_v = detail::is_complete_impl<T>::value;

  // ------------------------------------------
  // propagate_const

  namespace detail {

    template <class T, class U>
    struct propagate_const_impl
    {
      using type = T;
    };

    template <class T, class U>
    struct propagate_const_impl<T, const U>
    {
      using type = std::add_const_t<T>;
    };

  } // namespace detail

  /// conditionally add const to T depending on constness of U
  template <class T, class U>
  using propagate_const_t = typename detail::propagate_const_impl<T, U>::type;

}