//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/object_ptr.hpp>
#include <yave/rts/object_cast.hpp>
#include <yave/rts/dynamic_typing.hpp>
#include <yave/rts/bad_value_cast.hpp>

#include <exception>

namespace yave {

  /// value_cast
  ///
  /// dynamically cast object to specified value type.
  /// \throws bad_value_cast when fail.
  template <class T, class U>
  [[nodiscard]] object_ptr<propagate_const_t<T, U>>
    value_cast(const object_ptr<U>& obj)
  {
    if (likely(obj && has_type<T>(obj))) {
      using To = typename decltype(
        get_object_type(normalize_specifier(type_c<T>)))::type;
      return static_object_cast<propagate_const_t<To, U>>(obj);
    }
    throw bad_value_cast(obj ? get_type(obj) : nullptr, object_type<T>());
  }

  /// value_cast
  ///
  /// dynamically cast object to specified value type.
  /// \throws bad_value_cast when fail.
  template <class T, class U>
  [[nodiscard]] object_ptr<propagate_const_t<T, U>>
    value_cast(object_ptr<U>&& obj)
  {
    if (likely(obj && has_type<T>(obj))) {
      using To = typename decltype(
        get_object_type(normalize_specifier(type_c<T>)))::type;
      return static_object_cast<propagate_const_t<To, U>>(std::move(obj));
    }
    throw bad_value_cast(obj ? get_type(obj) : nullptr, object_type<T>());
  }

  /// value_cast_if
  ///
  /// dynamically cast object to specified value type.
  /// \returns nullptr when fail.
  template <class T, class U>
  [[nodiscard]] object_ptr<propagate_const_t<T, U>>
    value_cast_if(const object_ptr<U>& obj) noexcept
  {
    if (likely(obj && has_type<T>(obj))) {
      using To = typename decltype(
        get_object_type(normalize_specifier(type_c<T>)))::type;
      return static_object_cast<propagate_const_t<To, U>>(obj);
    }
    return nullptr;
  }

  /// value_cast_if
  ///
  /// dynamically cast object to specified value type.
  /// \returns nullptr when fail.
  template <class T, class U>
  [[nodiscard]] object_ptr<propagate_const_t<T, U>>
    value_cast_if(object_ptr<U>&& obj) noexcept
  {
    if (likely(obj && has_type<T>(obj))) {
      using To = typename decltype(
        get_object_type(normalize_specifier(type_c<T>)))::type;
      return static_object_cast<propagate_const_t<To, U>>(std::move(obj));
    }
    return nullptr;
  }

} // namespace yave