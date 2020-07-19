//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/object.hpp>
#include <yave/rts/object_ptr_storage.hpp>

namespace yave {

  // fwd
  template <class T, class U>
  [[nodiscard]] auto static_object_cast(object_ptr<U>&& o) noexcept
    -> object_ptr<T>;

  // ------------------------------------------
  // object_ptr

  /// Smart pointer to manage heap-allocated objects
  template <class T = Object>
  class object_ptr
  {
    // internal storage access
    template <class U>
    friend auto _get_storage(object_ptr<U>& ptr) noexcept
      -> object_ptr_storage&;
    template <class U>
    friend auto _get_storage(const object_ptr<U>& ptr) noexcept
      -> const object_ptr_storage&;

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
    [[nodiscard]] auto get() const noexcept -> pointer
    {
      return reinterpret_cast<pointer>(
        const_cast<propagate_const_t<Object*, pointer>>(m_storage.get()));
    }

    /// get address of member `value`
    /// \requires not null.
    [[nodiscard]] auto* value() const noexcept
    {
      assert(get());
      return &get()->value();
    }

    /// operator bool
    [[nodiscard]] explicit operator bool() const noexcept
    {
      return m_storage.get() != nullptr;
    }

    /// use_count
    /// \requires not null.
    [[nodiscard]] auto use_count() const noexcept -> uint64_t
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
    [[nodiscard]] auto release() noexcept -> pointer
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
    auto operator=(const object_ptr<element_type>& obj) noexcept
      -> object_ptr<element_type>&
    {
      object_ptr(obj).swap(*this);
      return *this;
    }

    /// operator=
    template <class U>
    auto operator=(const object_ptr<U>& obj) noexcept
      -> object_ptr<element_type>&
    {
      object_ptr(obj).swap(*this);
      return *this;
    }

    /// operator=
    auto operator=(object_ptr<element_type>&& obj) noexcept
      -> object_ptr<element_type>&
    {
      object_ptr(std::move(obj)).swap(*this);
      return *this;
    }

    /// operator=
    template <class U>
    auto operator=(object_ptr<U>&& obj) noexcept -> object_ptr<element_type>&
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

    // clone
    [[nodiscard]] auto clone() const -> object_ptr<std::remove_const_t<T>>;

    // destroy
    ~object_ptr() noexcept;

    /// atomic store
    void atomic_store(object_ptr<T> r, std::memory_order ord) noexcept
    {
      r.m_storage = {m_storage.atomic_exchange(r.m_storage.get(), ord)};
    }

    /// atomic load
    [[nodiscard]] auto atomic_load(std::memory_order ord) const noexcept
      -> object_ptr<T>
    {
      object_ptr<T> ret;
      ret.m_storage = {m_storage.atomic_load(ord)};
      ret.m_storage.increment_refcount();
      return ret;
    }

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

  inline void object_ptr_storage::decrement_refcount() const noexcept
  {
    if (likely(get() && !is_static())) {
      if (root_head()->refcount.fetch_sub() == 1) {
        std::atomic_thread_fence(std::memory_order_acquire);
        root_info_table()->destroy(get());
      }
    }
  }

  /// Clone object
  /// \effects Call copy constructor of the object from vtable.
  /// \returns `object_ptr<remove_const_t<T>>` pointing new object.
  /// \throws `std::bad_alloc` when `clone` returned nullptr.
  /// \notes Reference count of new object will be set to 1.
  /// \requires not null.
  template <class T>
  [[nodiscard]] auto object_ptr<T>::clone() const
    -> object_ptr<std::remove_const_t<T>>
  {
    assert(m_storage.get());

    object_ptr<Object> tmp =
      m_storage.this_info_table()->clone(m_storage.get());

    if (unlikely(!tmp))
      throw std::bad_alloc();

    using To = std::remove_const_t<T>;
    return static_object_cast<To>(std::move(tmp));
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
  [[nodiscard]] bool operator==(
    const object_ptr<T>& lhs,
    const object_ptr<U>& rhs) noexcept
  {
    return _get_storage(lhs).get() == _get_storage(rhs).get();
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
  [[nodiscard]] bool operator!=(
    const object_ptr<T>& lhs,
    const object_ptr<U>& rhs) noexcept
  {
    return _get_storage(lhs).get() != _get_storage(rhs).get();
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

  /// operator<
  template <class T, class U>
  [[nodiscard]] bool operator<(
    const object_ptr<T>& lhs,
    const object_ptr<U>& rhs) noexcept
  {
    return _get_storage(lhs).get() < _get_storage(rhs).get();
  }

  /// operator<=
  template <class T, class U>
  [[nodiscard]] bool operator<=(
    const object_ptr<T>& lhs,
    const object_ptr<U>& rhs) noexcept
  {
    return !(lhs > rhs);
  }

  /// operator>
  template <class T, class U>
  [[nodiscard]] bool operator>(
    const object_ptr<T>& lhs,
    const object_ptr<U>& rhs) noexcept
  {
    return _get_storage(lhs).get() > _get_storage(rhs).get();
  }

  /// operator>=
  template <class T, class U>
  [[nodiscard]] bool operator>=(
    const object_ptr<T>& lhs,
    const object_ptr<U>& rhs) noexcept
  {
    return !(lhs < rhs);
  }

  /// operator<
  template <class T>
  [[nodiscard]] bool operator<(const object_ptr<T>& lhs, nullptr_t) noexcept
  {
    return std::less<const Object*>()(_get_storage(lhs).get(), nullptr);
  }

  /// operator<
  template <class T>
  [[nodiscard]] bool operator<(nullptr_t, const object_ptr<T>& rhs) noexcept
  {
    return std::less<const Object*>()(nullptr, _get_storage(rhs).get());
  }

  /// operator<=
  template <class T>
  [[nodiscard]] bool operator<=(const object_ptr<T>& lhs, nullptr_t) noexcept
  {
    return !(lhs > nullptr);
  }

  /// operator<=
  template <class T>
  [[nodiscard]] bool operator<=(nullptr_t, const object_ptr<T>& rhs) noexcept
  {
    return !(nullptr > rhs);
  }

  /// operator>
  template <class T>
  [[nodiscard]] bool operator>(const object_ptr<T>& lhs, nullptr_t) noexcept
  {
    return std::greater<const Object*>()(_get_storage(lhs).get(), nullptr);
  }

  /// operator>
  template <class T>
  [[nodiscard]] bool operator>(nullptr_t, const object_ptr<T>& rhs) noexcept
  {
    return std::greater<const Object*>()(nullptr, _get_storage(rhs).get());
  }

  /// operator>=
  template <class T>
  [[nodiscard]] bool operator>=(const object_ptr<T>& lhs, nullptr_t) noexcept
  {
    return !(lhs < nullptr);
  }

  /// operator>=
  template <class T>
  [[nodiscard]] bool operator>=(nullptr_t, const object_ptr<T>& rhs) noexcept
  {
    return !(nullptr < rhs);
  }

  // ------------------------------------------
  // storage access

  /// internal storage accessor
  template <class U>
  [[nodiscard]] auto _get_storage(object_ptr<U>& obj) noexcept
    -> object_ptr_storage&
  {
    return obj.m_storage;
  }

  /// internal storage accessor
  template <class U>
  [[nodiscard]] auto _get_storage(const object_ptr<U>& obj) noexcept
    -> const object_ptr_storage&
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

  /// make object with custom allocator
  template <class T, class... Args>
  [[nodiscard]] auto make_object(std::pmr::memory_resource* mr, Args&&... args)
  {
    return object_ptr(detail::object_new<T>(mr, std::forward<Args>(args)...));
  }

  /// make object with default allocator
  template <class T, class... Args>
  [[nodiscard]] auto make_object(Args&&... args)
  {
    return make_object<T>(
      std::pmr::get_default_resource(), std::forward<Args>(args)...);
  }

} // namespace yave