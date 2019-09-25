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
  [[nodiscard]] auto value_cast(const object_ptr<U>& obj)
    -> object_ptr<propagate_const_t<T, U>>
  {
    // Apply
    if constexpr (std::is_same_v<std::decay_t<T>, Apply>) {
      if (likely(obj && _get_storage(obj).is_apply()))
        return static_object_cast<propagate_const_t<Apply, U>>(obj);
    }
    // Exception
    else if constexpr (std::is_same_v<std::decay_t<T>, Exception>) {
      if (likely(obj && _get_storage(obj).is_exception()))
        return static_object_cast<propagate_const_t<Exception, U>>(obj);
    }
    // general
    else if constexpr (is_tm_value(get_term<T>())) {
      if (likely(obj && has_type<T>(obj)))
        return static_object_cast<propagate_const_t<T, U>>(obj);
    } else
      static_assert(false_v<T>, "T is not value type");

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
    // Apply
    if constexpr (std::is_same_v<std::decay_t<T>, Apply>) {
      if (likely(obj && _get_storage(obj).is_apply()))
        return static_object_cast<propagate_const_t<Apply, U>>(std::move(obj));
    }
    // Exception
    else if constexpr (std::is_same_v<std::decay_t<T>, Exception>) {
      if (likely(obj && _get_storage(obj).is_exception()))
        return static_object_cast<propagate_const_t<Exception, U>>(
          std::move(obj));
    }
    // general
    else if constexpr (is_tm_value(get_term<T>())) {
      if (likely(obj && has_type<T>(obj)))
        return static_object_cast<propagate_const_t<T, U>>(std::move(obj));
    } else
      static_assert(false_v<T>, "T is not value type");

    throw bad_value_cast(get_type(obj), object_type<T>());
  }

  /// value_cast_if
  ///
  /// dynamically cast object to specified value type.
  /// \returns nullptr when fail.
  template <class T, class U>
  [[nodiscard]] auto value_cast_if(const object_ptr<U>& obj) noexcept
    -> object_ptr<propagate_const_t<T, U>>
  {
    // Apply
    if constexpr (std::is_same_v<std::decay_t<T>, Apply>) {
      if (likely(obj && _get_storage(obj).is_apply()))
        return static_object_cast<propagate_const_t<Apply, U>>(obj);
    }
    // Exception
    else if constexpr (std::is_same_v<std::decay_t<T>, Exception>) {
      if (likely(obj && _get_storage(obj).is_exception()))
        return static_object_cast<propagate_const_t<Exception, U>>(obj);
    }
    // general
    else if constexpr (is_tm_value(get_term<T>())) {
      if (likely(obj && has_type<T>(obj)))
        return static_object_cast<propagate_const_t<T, U>>(obj);
    } else
      static_assert(false_v<T>, "T is not value");

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
    // Apply
    if constexpr (std::is_same_v<std::decay_t<T>, Apply>) {
      if (likely(obj && _get_storage(obj).is_apply()))
        return static_object_cast<propagate_const_t<Apply, U>>(std::move(obj));
    }
    // Exception
    else if constexpr (std::is_same_v<std::decay_t<T>, Exception>) {
      if (likely(obj && _get_storage(obj).is_exception()))
        return static_object_cast<propagate_const_t<Exception, U>>(
          std::move(obj));
    }
    // general
    else if constexpr (is_tm_value(get_term<T>())) {
      if (likely(obj && has_type<T>(obj)))
        return static_object_cast<propagate_const_t<T, U>>(std::move(obj));
    } else
      static_assert(false_v<T>, "T is not value type");

    return nullptr;
  }

} // namespace yave