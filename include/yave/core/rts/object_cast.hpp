//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/rts/object_ptr.hpp>

namespace yave {

  /// static object cast (possibly unsafe)
  template <class T, class U>
  [[nodiscard]] object_ptr<T> static_object_cast(const object_ptr<U>& obj)
  {
    // add refcount
    if (likely(obj && !obj.is_static()))
      _get_storage(obj).head()->refcount.fetch_add();

    // to ensure safety of cast
    static_assert(
      std::is_base_of_v<T, U> || // upcast
      std::is_base_of_v<U, T> || // downcast
      sizeof(T) == sizeof(U));   // crosscast to proxy type

    // crosscasting to proxy types to add additional compile time information
    // using static_cast leads undefined behaviour by itself. So here we use
    // reinterpret_cast. Programmer is responsible for casting back to original
    // type or it's base type (Object for example) before accessing it's data.
    return reinterpret_cast<T*>(obj.get());
  }

  /// static object cast (possibly unsafe)
  template <class T, class U>
  [[nodiscard]] object_ptr<T> static_object_cast(object_ptr<U>&& obj)
  {
    static_assert(
      std::is_base_of_v<T, U> || // upcast
      std::is_base_of_v<U, T> || // downcast
      sizeof(T) == sizeof(U));   // crosscast to proxy type
    return reinterpret_cast<T*>(obj.release());
  }

} // namespace yave
