//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <type_traits>

#define YAVE_DECL_ENUM_FLAG(TYPE)              \
  namespace yave {                             \
    template <>                                \
    struct is_enum_flag<TYPE> : std::true_type \
    {                                          \
    };                                         \
  }

namespace yave {

  /// create enum class from value
  template <class E>
  constexpr std::underlying_type_t<E> to_underlying_type(E e)
  {
    return static_cast<std::underlying_type_t<E>>(e);
  }

  /// trait class to use SFINAE on enum class types
  template <class T>
  struct is_enum_flag : std::false_type
  {
  };

  /// alias
  template <class T>
  static constexpr bool is_enum_flag_v = is_enum_flag<T>::value;

  /// operator| for enum class
  template <
    class Flag,
    std::enable_if_t<is_enum_flag_v<Flag>, std::nullptr_t> = nullptr>
  constexpr Flag operator|(Flag lhs, Flag rhs)
  {
    return static_cast<Flag>(to_underlying_type(lhs) | to_underlying_type(rhs));
  }

  /// operator|= for enum class
  template <
    class Flag,
    std::enable_if_t<is_enum_flag_v<Flag>, std::nullptr_t> = nullptr>
  constexpr Flag& operator|=(Flag& lhs, Flag rhs)
  {
    lhs = lhs | rhs;
    return lhs;
  }

  /// operator& for enum class
  template <
    class Flag,
    std::enable_if_t<is_enum_flag_v<Flag>, std::nullptr_t> = nullptr>
  constexpr Flag operator&(Flag lhs, Flag rhs)
  {
    return static_cast<Flag>(to_underlying_type(lhs) & to_underlying_type(rhs));
  }

  /// operator&= for enum class
  template <
    class Flag,
    std::enable_if_t<is_enum_flag_v<Flag>, std::nullptr_t> = nullptr>
  constexpr Flag& operator&=(Flag& lhs, Flag rhs)
  {
    lhs = lhs & rhs;
    return lhs;
  }

  /// operator^ for enum class
  template <
    class Flag,
    std::enable_if_t<is_enum_flag_v<Flag>, std::nullptr_t> = nullptr>
  constexpr Flag operator^(Flag lhs, Flag rhs)
  {
    return static_cast<Flag>(to_underlying_type(lhs) ^ to_underlying_type(rhs));
  }

  /// operator^= for enum class
  template <
    class Flag,
    std::enable_if_t<is_enum_flag_v<Flag>, std::nullptr_t> = nullptr>
  constexpr Flag& operator^=(Flag& lhs, Flag rhs)
  {
    lhs = lhs ^ rhs;
    return lhs;
  }

  /// operator~ for enum class
  template <
    class Flag,
    std::enable_if_t<is_enum_flag_v<Flag>, std::nullptr_t> = nullptr>
  constexpr Flag operator~(Flag flag)
  {
    return static_cast<Flag>(~(to_underlying_type(flag)));
  }

  /// operator!
  /// for conversion to boolean.
  template <
    class Flag,
    std::enable_if_t<is_enum_flag_v<Flag>, std::nullptr_t> = nullptr>
  constexpr bool operator!(Flag flag)
  {
    return !(to_underlying_type(flag));
  }
} // namespace yave
