//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/object.hpp>
#include <yave/rts/info_table_tags.hpp>

#include <cstring>

namespace yave {

  struct object_info_table; // defined in object_ptr.hpp

  /// internal storage of object_ptr
  struct object_ptr_storage
  {
    /// get pointer
    [[nodiscard]] auto get() const noexcept -> const Object*
    {
      return m_ptr;
    }

    /// get address of header of this object.
    [[nodiscard]] auto this_head() const noexcept -> const Object*
    {
      assert(get());
      return get();
    }

    /// get address of heaedr of root object.
    [[nodiscard]] auto root_head() const noexcept -> const Object*
    {
      assert(get());
      auto* this_ptr = get();
      auto* root_ptr =
        (const Object*)(((const char*)this_ptr) - this_ptr->offset);
      return root_ptr;
    }

    /// get info table pointer
    [[nodiscard]] auto this_info_table() const noexcept
      -> const object_info_table*
    {
      assert(get());
      return detail::clear_info_table_tag(this_head()->info_table);
    }

    /// get info table of root object
    [[nodiscard]] auto root_info_table() const noexcept
      -> const object_info_table*
    {
      assert(get());
      return detail::clear_info_table_tag(root_head()->info_table);
    }

    /// check if current info table tag matches type T's tag
    template <class T>
    [[nodiscard]] bool match_info_table_tag() const noexcept
    {
      return detail::check_info_table_tag(
        this_head()->info_table, detail::get_info_table_tag<T>());
    }

    /// static?
    [[nodiscard]] bool is_static() const noexcept
    {
      assert(get());
      // inherit from root object
      return root_head()->refcount.load_relaxed() == 0;
    }

    /// use_count
    [[nodiscard]] auto use_count() const noexcept -> uint64_t
    {
      assert(get());
      return root_head()->refcount.load_relaxed();
    }

    /// increment refcount (mutable)
    void increment_refcount() const noexcept
    {
      if (likely(get() && !is_static()))
        root_head()->refcount.fetch_add();
    }

    /// decrement refcount (mutable)
    void decrement_refcount() const noexcept; // defined in object_ptr.hpp

    /// atomic store with memory order
    void atomic_store(const Object* ptr, std::memory_order ord) noexcept
    {
      const auto ref = std::atomic_ref(m_ptr);
      ref.store(ptr, ord);
    }

    /// atomic load with memory order
    [[nodiscard]] auto atomic_load(std::memory_order ord) const noexcept
    {
      const auto ref = std::atomic_ref(const_cast<const Object*&>(m_ptr));
      return ref.load(ord);
    }

    /// atomic exchange with memory order
    [[nodiscard]] auto atomic_exchange(
      const Object* ptr,
      std::memory_order ord) noexcept
    {
      auto ref = std::atomic_ref(m_ptr);
      return ref.exchange(ptr, ord);
    }

  public:
    /// Ctor
    template <class T>
    constexpr object_ptr_storage(T* p) noexcept
      : m_ptr {static_cast<const Object*>(p)}
    {
    }

    /// Ctor
    constexpr object_ptr_storage(nullptr_t p) noexcept
      : m_ptr {p}
    {
    }

    /// Copy ctor
    object_ptr_storage(const object_ptr_storage& other) noexcept
      : m_ptr {other.m_ptr}
    {
    }

    /// Copy assign
    auto& operator=(const object_ptr_storage& other) noexcept
    {
      m_ptr = other.m_ptr;
      return *this;
    }

  private:
    /// pointer to object
    const Object* m_ptr;
  };

  // clang-format off
  static_assert(std::atomic_ref<Object*>::is_always_lock_free);
  static_assert(std::atomic_ref<Object*>::required_alignment == alignof(Object*));
  static_assert(std::is_standard_layout_v<object_ptr_storage>);
  static_assert(sizeof(object_ptr_storage) == sizeof(Object*));
  static_assert(alignof(object_ptr_storage) == alignof(Object*));
  static_assert(std::is_nothrow_copy_constructible_v<object_ptr_storage>);
  static_assert(std::is_nothrow_move_constructible_v<object_ptr_storage>);
  // clang-format on

} // namespace yave