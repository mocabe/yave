//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/object.hpp>

#include <cstring>

namespace yave {

  struct object_info_table; // defined in object_ptr.hpp

  /// internal storage of object_ptr
  struct object_ptr_storage
  {
    /// pointer tag values. stored in lowest 3 bits of 64bit data.
    enum class pointer_tags : uint64_t
    {
      // clang-format off

      // h <---------------------> l
      //                        |<>|
      //                        tag (3bit)
      //                        ^^^^^^^^^^

      pointer   = 0x0, //< plain pointer
      exception = 0x1, //< exception returned from vtbl_code_func

      extract_mask = 0x0000000000000007, // 0...0111
      clear_mask   = 0xFFFFFFFFFFFFFFF8, // 1...1000

      // clang-format on
    };

    /// extract pointer tag flag
    [[nodiscard]] auto get_pointer_tag() const noexcept -> pointer_tags
    {
      uint64_t tag;
      std::memcpy(&tag, &m_tag, sizeof(tag));
      return static_cast<pointer_tags>(
        tag & static_cast<uint64_t>(pointer_tags::extract_mask));
    }

    /// set new pointer tag flag
    void set_pointer_tag(pointer_tags tag) noexcept
    {
      // TODO: use std::declare_reachable for tagged pointer?
      uint64_t tmp;
      std::memcpy(&tmp, &m_tag, sizeof(tmp));
      tmp &= static_cast<uint64_t>(pointer_tags::clear_mask);
      tmp |= static_cast<uint64_t>(tag);
      std::memcpy(&m_tag, &tmp, sizeof(m_tag));
    }

    void clear_pointer_tag() noexcept
    {
      uint64_t tmp;
      std::memcpy(&tmp, &m_tag, sizeof(tmp));
      tmp &= static_cast<uint64_t>(pointer_tags::clear_mask);
      std::memcpy(&m_tag, &tmp, sizeof(m_tag));
    }

    /// get pointer
    /// Ideally, optimized into single AND instruction.
    [[nodiscard]] auto get() const noexcept -> const Object*
    {
      // load as non-pointer
      uint64_t tag;
      std::memcpy(&tag, &m_tag, sizeof(tag));
      // clear pointer tag
      tag &= static_cast<uint64_t>(pointer_tags::clear_mask);
      // cast to pointer
      const Object* ptr;
      std::memcpy(&ptr, &tag, sizeof(ptr));
      return ptr;
    }

    /// get address of header
    [[nodiscard]] auto head() const noexcept -> const Object*
    {
      assert(get());
      return get();
    }

    /// get info table pointer
    [[nodiscard]] auto info_table() const noexcept -> const object_info_table*
    {
      assert(get());
      return head()->info_table;
    }

    /// exception? (optional)
    [[nodiscard]] bool is_exception() const noexcept
    {
      return get_pointer_tag() == pointer_tags::exception;
    }

    /// static?
    [[nodiscard]] bool is_static() const noexcept
    {
      assert(get());
      return head()->refcount.load() == 0;
    }

    /// use_count
    [[nodiscard]] auto use_count() const noexcept -> uint64_t
    {
      assert(get());
      return head()->refcount.load();
    }

    /// increment refcount
    void increment_refcount() noexcept
    {
      if (likely(get() && !is_static()))
        head()->refcount.fetch_add();
    }

    /// decrement refcount
    void decrement_refcount() noexcept; // defined in object_ptr.hpp

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
    union
    {
      /// pointer to object
      const Object* m_ptr;
      /// pointer tag
      uint64_t m_tag;
    };
  };

  // should be standard layout
  static_assert(std::is_standard_layout_v<object_ptr_storage>);
  static_assert(std::is_trivially_copy_constructible_v<object_ptr_storage>);
  static_assert(std::is_trivially_copy_assignable_v<object_ptr_storage>);
  static_assert(std::is_trivially_copyable_v<object_ptr_storage>);

} // namespace yave