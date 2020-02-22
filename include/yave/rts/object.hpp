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
#include <memory_resource>

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

  /// Base class of heap-allocated objects.
  /// You should ALWAYS initialized members from derived classes, and handle
  /// copying.
  struct Object
  {
    /// term
    static constexpr auto term = type_c<tm_value<Object>>;

    /// Ctor
    constexpr Object(
      uint32_t rc,
      uint32_t off,
      const object_info_table* it,
      std::pmr::memory_resource* mr)
      : refcount {rc}
      , offset {off}
      , info_table {it}
      , memory_resource {mr}
    {
    }

    /// Copy ctor
    constexpr Object(const Object& other)
      : refcount {1} // always init as 1
      , offset {other.offset}
      , info_table {other.info_table}
      , memory_resource {other.memory_resource}
    {
    }

    void* operator new(size_t)    = delete;
    void operator delete(void*)   = delete;
    void* operator new[](size_t)  = delete;
    void operator delete[](void*) = delete;

    /// operator new using memory_resource
    void* operator new(
      size_t /* see below */,
      size_t size,
      std::pmr::memory_resource* mr)
    {
      // The first parameter of this function is not used.
      // Since overloading resolution rule on placement operator delete when
      // constructor failed by throwing exception only accepts overloading which
      // has the same number of arguments to the placement operator new, we need
      // to match the number of arguments by adding another argument, and
      // manually pass size into second parameter.

      std::pmr::polymorphic_allocator<std::byte> alloc(mr);
      return alloc.allocate(size);
    }

    /// operator delete using memory_resource
    void operator delete(void* p, size_t size, std::pmr::memory_resource* mr)
    {
      std::pmr::polymorphic_allocator<std::byte> alloc(mr);
      alloc.deallocate((std::byte*)p, size);
    }

    /// 4byte: reference count
    mutable atomic_refcount<uint32_t> refcount;

    /// 4byte: offset of this object
    const uint32_t offset;

    /// 8byte: pointer to info table
    const object_info_table* info_table;

    /// 8byte: pointer for polymorphic memory resource
    /// This pointer should only be used from vtable functions
    std::pmr::memory_resource* memory_resource;
  };

  static_assert(std::is_standard_layout_v<Object>);

  // ------------------------------------------
  // object_new/object_delete

  /// Construct new object from arguments, with its memory allocated from
  /// memory_resource.
  template <class T, class... Args>
  [[nodiscard]] T* object_new(std::pmr::memory_resource* mr, Args&&... args)
  {
      using newT = std::remove_const_t<T>;
      auto p     = new (sizeof(T), mr) newT(std::forward<Args>(args)...);
    p->memory_resource = mr;
    return p;
  }

  /// Destruct and delete object with its own memory_resource.
    /// Template parameter T should be the type of actual object allocated.
    /// This function is used in vtbl_destroy() to destroy object via vtable,
    /// which knows T at compile time.
  template <class T>
  void object_delete(const T* p)
  {
    auto mr = p->memory_resource;
    p->~T();
    T::operator delete((void*)p, sizeof(T), mr);
  }

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
