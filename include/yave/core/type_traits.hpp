//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

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

  /// always returns false
  template <class... Ts>
  static constexpr bool false_v = make_false<Ts...>::value;

  // ------------------------------------------
  // is_complete_v

  /// detect complete type
  template <class T, auto _x = []() -> void {}>
  concept is_complete_v = requires { sizeof(T); _x; };

  // ------------------------------------------
  // propagate_const

  /// conditionally add const to T depending on constness of U
  template <class T, class U>
  using propagate_const_t = std::conditional_t<std::is_const_v<U>, std::add_const_t<T>, T>;

}