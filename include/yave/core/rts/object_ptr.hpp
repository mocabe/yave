//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/rts/object.hpp>
#include <yave/core/rts/object_ptr_storage.hpp>

namespace yave {

  // ------------------------------------------
  // object_ptr

  /// Smart pointer to manage heap-allocated objects
  template <class T = Object>
  class object_ptr
  {
    // internal storage access
    template <class U>                             //
    friend object_ptr_storage&                     //
      _get_storage(object_ptr<U>&) noexcept;       //
                                                   //
    template <class U>                             //
    friend const object_ptr_storage&               //
      _get_storage(const object_ptr<U>&) noexcept; //

  public:
    // value type
    using element_type = T;
    // pointer
    using pointer = T*;

    /// Constructor
    constexpr object_ptr() noexcept
      : m_storage {nullptr}
    {
    }

    /// Constructor
    constexpr object_ptr(nullptr_t) noexcept
      : m_storage {nullptr}
    {
    }

    /// Pointer constructor
    constexpr object_ptr(pointer p) noexcept
      : m_storage {p}
    {
    }

    /// Copy constructor
    /// \effects increases reference count.
    object_ptr(const object_ptr& other) noexcept
      : m_storage {other.m_storage}
    {
      m_storage.increment_refcount();
    }

    /// Move constructor
    object_ptr(object_ptr&& other) noexcept
      : m_storage {other.m_storage}
    {
      other.m_storage = {nullptr};
    }

    /// Copy convert constructor
    /// \effects increases reference count.
    template <class U, class = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    object_ptr(const object_ptr<U>& other) noexcept
      : m_storage {_get_storage(other)}
    {
      m_storage.increment_refcount();
    }

    /// Move convert constructor
    template <class U, class = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    object_ptr(object_ptr<U>&& other) noexcept
      : m_storage {_get_storage(other)}
    {
      _get_storage(other) = {nullptr};
    }

    /// get address of object
    [[nodiscard]] pointer get() const noexcept
    {
      return reinterpret_cast<pointer>(
        const_cast<propagate_const_t<Object*, pointer>>(m_storage.get()));
    }

    /// get address of member `value`
    /// \requires not null.
    [[nodiscard]] auto* value() const noexcept
    {
      assert(get());
      return &get()->value;
    }

    /// operator bool
    [[nodiscard]] explicit operator bool() const noexcept
    {
      return m_storage.get() != nullptr;
    }

    /// use_count
    /// \requires not null.
    [[nodiscard]] uint64_t use_count() const noexcept
    {
      return m_storage.use_count();
    }

    /// is_static
    /// \requires not null.
    [[nodiscard]] bool is_static() const noexcept
    {
      return m_storage.is_static();
    }

    /// release pointer
    /// \effects get() return nullptr after call
    [[nodiscard]] pointer release() noexcept
    {
      auto tmp  = get();
      m_storage = {nullptr};
      return tmp;
    }

    /// swap data
    void swap(object_ptr<element_type>& obj) noexcept
    {
      std::swap(obj.m_storage, m_storage);
    }

    /// operator=
    object_ptr<element_type>&
      operator=(const object_ptr<element_type>& obj) noexcept
    {
      object_ptr(obj).swap(*this);
      return *this;
    }

    /// operator=
    template <class U>
    object_ptr<element_type>& operator=(const object_ptr<U>& obj) noexcept
    {
      object_ptr(obj).swap(*this);
      return *this;
    }

    /// operator=
    object_ptr<element_type>& operator=(object_ptr<element_type>&& obj) noexcept
    {
      object_ptr(std::move(obj)).swap(*this);
      return *this;
    }

    /// operator=
    template <class U>
    object_ptr<element_type>& operator=(object_ptr<U>&& obj) noexcept
    {
      object_ptr(std::move(obj)).swap(*this);
      return *this;
    }

    /// operator*
    [[nodiscard]] auto& operator*() const noexcept
    {
      return *value();
    }

    /// operator->
    [[nodiscard]] auto* operator-> () const noexcept
    {
      return value();
    }

    // destroy
    ~object_ptr() noexcept;

  private:
    /// pointer to object
    object_ptr_storage m_storage;
  };

  /// array version (disabled)
  template <class T, size_t N>
  class object_ptr<T[N]>; // = delete

  // ------------------------------------------
  // object_info_table

  /// Object info table
  struct object_info_table
  {
    /// pointer to type object
    object_ptr<const Type> obj_type;
    /// total size of object
    uint64_t obj_size;
    /// name of object (or UUID)
    const char* obj_name;
    /// vtable of delete function
    void (*destroy)(const Object*) noexcept;
    /// vtable of clone function
    Object* (*clone)(const Object*)noexcept;
  };

  // ------------------------------------------
  // object_ptr::~object_ptr()

  inline void object_ptr_storage::decrement_refcount() noexcept
  {
    if (likely(get() && !is_static())) {
      if (head()->refcount.fetch_sub() == 1) {
        std::atomic_thread_fence(std::memory_order_acquire);
        info_table()->destroy(get());
      }
    }
  }

  /// Destructor
  /// \effects Destroy object with vtable function when reference count become
  /// 0
  template <class T>
  object_ptr<T>::~object_ptr() noexcept
  {
    m_storage.decrement_refcount();
  }

  // ------------------------------------------
  // operators

  /// operator==
  template <class T, class U>
  [[nodiscard]] bool
    operator==(const object_ptr<T>& lhs, const object_ptr<U>& rhs) noexcept
  {
    return lhs.get() == rhs.get();
  }

  /// operator==
  template <class T>
  [[nodiscard]] bool operator==(nullptr_t, const object_ptr<T>& p) noexcept
  {
    return !p;
  }

  /// operator==
  template <class T>
  [[nodiscard]] bool operator==(const object_ptr<T>& p, nullptr_t) noexcept
  {
    return !p;
  }

  /// operator!=
  template <class T, class U>
  [[nodiscard]] bool
    operator!=(const object_ptr<T>& lhs, const object_ptr<U>& rhs) noexcept
  {
    return lhs.get() != rhs.get();
  }

  /// operator!=
  template <class T>
  [[nodiscard]] bool operator!=(nullptr_t, const object_ptr<T>& p) noexcept
  {
    return static_cast<bool>(p);
  }

  /// operator!=
  template <class T>
  [[nodiscard]] bool operator!=(const object_ptr<T>& p, nullptr_t) noexcept
  {
    return static_cast<bool>(p);
  }

  // ------------------------------------------
  // storage access

  /// internal storage accessor
  template <class U>
  [[nodiscard]] object_ptr_storage& //
    _get_storage(object_ptr<U>& obj) noexcept
  {
    return obj.m_storage;
  }

  /// internal storage accessor
  template <class U>
  [[nodiscard]] const object_ptr_storage& //
    _get_storage(const object_ptr<U>& obj) noexcept
  {
    return obj.m_storage;
  }

  // ------------------------------------------
  // deduction guides

  // nullptr
  object_ptr(nullptr_t)->object_ptr<Object>;
  // default
  object_ptr()->object_ptr<Object>;

  // ------------------------------------------
  // make_object

  /// make object
  template <class T, class... Args>
  [[nodiscard]] auto make_object(Args&&... args)
  {
    return object_ptr<T>(new T(std::forward<Args>(args)...));
  }

  // ------------------------------------------
  // misc

  template <class T>
  [[nodiscard]] object_ptr<T> clear_pointer_tag(object_ptr<T> obj)
  {
    _get_storage(obj).clear_pointer_tag();
    return obj;
  }

} // namespace yave