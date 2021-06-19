//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>

namespace yave {

  // ------------------------------------------
  // bool constants

  /// true_c
  static constexpr std::true_type true_c {};
  /// false_c
  static constexpr std::false_type false_c {};

  // ------------------------------------------
  // meta_type

  /// meta_type
  template <class T>
  struct meta_type
  {
    using type = T;
  };

  /// meta_type constant
  template <class T>
  static constexpr meta_type<T> type_c {};

  /// operator==
  template <class T1, class T2>
  [[nodiscard]] constexpr auto operator==(meta_type<T1>, meta_type<T2>)
  {
    if constexpr (std::is_same_v<T1, T2>)
      return true_c;
    else
      return false_c;
  }

  /// operator!=
  template <class T1, class T2>
  [[nodiscard]] constexpr auto operator!=(meta_type<T1> lhs, meta_type<T2> rhs)
  {
    return std::bool_constant<!(lhs == rhs)> {};
  }
}