//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/rts/object_ptr.hpp>

namespace yave {

  /// static object cast
  template <class T, class U>
  [[nodiscard]] object_ptr<T> static_object_cast(const object_ptr<U>& obj)
  {
    // add refcount
    if (likely(obj && !obj.is_static()))
      _get_storage(obj).head()->refcount.fetch_add();

    if constexpr (std::is_base_of_v<U, T> && sizeof(T) == sizeof(U))
      // downcast to proxy types (empty derived class from Object) using
      // static_cast IS undefined behaviour. reinterpret_cast is probably
      // undefined behaviour too but it works in practice. object_ptr uses
      // `const Object*` pointer in underlying storage so most of read/write
      // operations are still done through `const Object*` pointer.
      return reinterpret_cast<T*>(obj.get());
    else
      // it's undefined behavior if `obj` is not an actual object of T.
      return static_cast<T*>(obj.get());
  }

  /// static object cast
  template <class T, class U>
  [[nodiscard]] object_ptr<T> static_object_cast(object_ptr<U>&& obj)
  {
    if constexpr (std::is_base_of_v<U, T> && sizeof(T) == sizeof(U))
      return reinterpret_cast<T*>(obj.release());
    else
      return static_cast<T*>(obj.release());
  }

} // namespace yave
