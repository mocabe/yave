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

  // ------------------------------------------
  // Object

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

  // ------------------------------------------
  // is_object_pointer

  namespace detail {

    template <class T>
    struct is_object_pointer_impl : std::false_type
    {
    };

    template <class T>
    struct is_object_pointer_impl<T*>
    {
      static constexpr bool value =
        (std::is_base_of_v<Object, T>) ? true : false;
    };

    template <class T>
    struct is_object_pointer_impl<object_ptr<T>> : std::true_type
    {
    };

  } // namespace detail

  /// Heap object pointer? (raw pointer or object_ptrT>)
  template <class T>
  inline constexpr bool is_object_pointer_v =
    detail::is_object_pointer_impl<std::decay_t<T>>::value;

  // ------------------------------------------
  // object_pointer_element_t

  namespace detail {

    template <class T>
    struct object_pointer_element_impl_
    {
    };

    template <class T>
    struct object_pointer_element_impl_<T*>
    {
      using type = T;
    };

    template <class T>
    struct object_pointer_element_impl_<object_ptr<T>>
    {
      using type = T;
    };

    template <class T, class = void>
    struct object_pointer_element_impl
    {
    };

    template <class T>
    struct object_pointer_element_impl<
      T,
      std::enable_if_t<is_object_pointer_v<T>>>
    {
      using type = typename object_pointer_element_impl_<std::decay_t<T>>::type;
    };
  } // namespace detail

  /// Element type of pointer
  template <class T>
  using object_pointer_element_t =
    typename detail::object_pointer_element_impl<T>::type;

} // namespace yave
