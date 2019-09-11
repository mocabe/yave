//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/atomic.hpp>
#include <yave/rts/terms.hpp>

#include <cstdint>
#include <atomic>
#include <cassert>
#include <stdexcept>
#include <new>

namespace yave {

  // value of runtime type
  class type_value;

  // object info table
  struct object_info_table;

  // heap-allocated object of type T
  template <class T>
  struct Box;

  // handler for heap-allocated object
  template <class>
  class object_ptr;

  // heap-allocated runtime type infomation
  using Type = Box<type_value>;

  /// Base class of heap-allocated objects
  struct Object
  {
    /// term
    static constexpr auto term = type_c<tm_value<Object>>;

    /// Ctor
    constexpr Object(const object_info_table* info)
      : info_table {info}
    {
      /* Default initialize refcount and spinlock */
    }

    /// Copy ctor
    constexpr Object(const Object& other)
      : info_table {other.info_table}
    {
      /* Default initialize refcount and spinlock */
    }

    /// 4byte: reference count
    mutable atomic_refcount<uint32_t> refcount = {1u};

    /// 1byte: spinlock
    mutable atomic_spinlock<uint8_t> spinlock = {/*false*/};

    /* 3byte: padding */
    std::byte reserved[3] = {};

    /// 8byte: pointer to info table
    const object_info_table* info_table;
  };

  static_assert(std::is_standard_layout_v<Object>);

} // namespace yave
