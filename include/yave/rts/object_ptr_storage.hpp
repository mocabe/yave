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

    [[nodiscard]] auto root_info_table() const noexcept
      -> const object_info_table*
    {
      assert(get());
      return detail::clear_info_table_tag(root_head()->info_table);
    }

    /// exception?
    [[nodiscard]] bool is_exception() const noexcept
    {
      return detail::has_exception_tag(this_head()->info_table);
    }

    /// apply?
    [[nodiscard]] bool is_apply() const noexcept
    {
      return detail::has_apply_tag(this_head()->info_table);
    }

    /// lambda?
    [[nodiscard]] bool is_lambda() const noexcept
    {
      return detail::has_lambda_tag(this_head()->info_table);
    }

    /// variable?
    [[nodiscard]] bool is_variable() const noexcept
    {
      return detail::has_variable_tag(this_head()->info_table);
    }

    /// static?
    [[nodiscard]] bool is_static() const noexcept
    {
      assert(get());
      // inherit from root object
      return root_head()->refcount.load() == 0;
    }

    /// use_count
    [[nodiscard]] auto use_count() const noexcept -> uint64_t
    {
      assert(get());
      return root_head()->refcount.load();
    }

    /// increment refcount (mutable)
    void increment_refcount() const noexcept
    {
      if (likely(get() && !is_static()))
        root_head()->refcount.fetch_add();
    }

    /// decrement refcount (mutable)
    void decrement_refcount() const noexcept; // defined in object_ptr.hpp

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

  private:
    /// pointer to object
    const Object* m_ptr;
  };

  // should be standard layout
  static_assert(std::is_standard_layout_v<object_ptr_storage>);
  static_assert(std::is_trivially_copy_constructible_v<object_ptr_storage>);
  static_assert(std::is_trivially_copy_assignable_v<object_ptr_storage>);
  static_assert(std::is_trivially_copyable_v<object_ptr_storage>);

} // namespace yave