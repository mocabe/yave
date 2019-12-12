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

  /// value_cast_if
  ///
  /// dynamically cast object to specified value type.
  /// \returns nullptr when fail.
  template <class T, class U>
  [[nodiscard]] auto value_cast_if(const object_ptr<U>& obj) noexcept
    -> object_ptr<propagate_const_t<T, U>>
  {
    if (has_type<T>(obj))
      return static_object_cast<propagate_const_t<T, U>>(obj);

    return nullptr;
  }

  /// value_cast_if
  ///
  /// dynamically cast object to specified value type.
  /// \returns nullptr when fail.
  template <class T, class U>
  [[nodiscard]] auto value_cast_if(object_ptr<U>&& obj) noexcept
    -> object_ptr<propagate_const_t<T, U>>
  {
    if (has_type<T>(obj))
      return static_object_cast<propagate_const_t<T, U>>(std::move(obj));

    return nullptr;
  }

  /// value_cast
  ///
  /// dynamically cast object to specified value type.
  /// \throws bad_value_cast when fail.
  template <class T, class U>
  [[nodiscard]] auto value_cast(const object_ptr<U>& obj)
    -> object_ptr<propagate_const_t<T, U>>
  {
    if (auto r = value_cast_if<T>(obj))
      return r;

    throw bad_value_cast(get_type(obj), object_type<T>());
  }

  /// value_cast
  ///
  /// dynamically cast object to specified value type.
  /// \throws bad_value_cast when fail.
  template <class T, class U>
  [[nodiscard]] auto value_cast(object_ptr<U>&& obj)
    -> object_ptr<propagate_const_t<T, U>>
  {
    if (auto r = value_cast_if<T>(std::move(obj)))
      return r;

    throw bad_value_cast(get_type(obj), object_type<T>());
  }

} // namespace yave