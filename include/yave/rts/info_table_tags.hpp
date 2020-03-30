//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <cstring>

namespace yave {

  // fwd
  struct object_info_table;
  template <class T>
  struct object_type_traits;

  namespace detail {

    // Info table pointer tags.
    // Uses lowest 3 bits of info table pointer.
    enum class info_table_tags : uintptr_t
    {
      // h <---------------------> l
      //                        |<>|
      //                        tag (3bit)
      //                        ^^^^^^^^^^
      vanilla      = 0,                  //
      _1           = 1,                  // Exception
      _2           = 2,                  // Apply
      _3           = 3,                  // Variable
      _4           = 4,                  // Lambda
      _5           = 5,                  //
      _6           = 6,                  //
      _7           = 7,                  //
      extract_mask = 0x0000000000000007, // 0...0111
      clear_mask   = 0xFFFFFFFFFFFFFFF8, // 1...1000
    };

    template <class T, class = void>
    struct has_info_table_tag_impl : std::false_type
    {
    };

    template <class T>
    struct has_info_table_tag_impl<
      T,
      std::void_t<decltype(object_type_traits<T>::info_table_tag)>>
      : std::true_type
    {
    };

    /// check custom info table tag
    template <class T>
    [[nodiscard]] constexpr auto has_info_table_tag()
    {
      return has_info_table_tag_impl<T>::value;
    }

    /// Get info table tag of give type
    template <class T>
    [[nodiscard]] constexpr auto get_info_table_tag()
    {
      static_assert(has_info_table_tag<T>());
      return object_type_traits<T>::info_table_tag;
    }

    /// Add info table tag of type T to given pointer.
    [[nodiscard]] inline auto add_info_table_tag(
      const object_info_table* ptr,
      info_table_tags tag) -> const object_info_table*
    {
      uintptr_t tmp = 0;
      std::memcpy(&tmp, &ptr, sizeof(ptr));

      tmp |= static_cast<uintptr_t>(tag);

      const object_info_table* ret = nullptr;
      std::memcpy(&ret, &tmp, sizeof(ret));
      return ret;
    }

    /// Clear info table pointer tag and get actual address
    [[nodiscard]] inline auto clear_info_table_tag(
      const object_info_table* tagged) -> const object_info_table*
    {
      uintptr_t tmp = 0;
      std::memcpy(&tmp, &tagged, sizeof(tagged));

      tmp &= static_cast<uintptr_t>(info_table_tags::clear_mask);

      const object_info_table* ret = nullptr;
      std::memcpy(&ret, &tmp, sizeof(ret));
      return ret;
    }

    /// Check if info table pointer has tag value of type T.
    [[nodiscard]] inline bool check_info_table_tag(
      const object_info_table* tagged,
      info_table_tags tag)
    {
      uintptr_t tmp = 0;
      std::memcpy(&tmp, &tagged, sizeof(tagged));
      tmp &= static_cast<uintptr_t>(info_table_tags::extract_mask);
      return tmp == static_cast<uintptr_t>(tag);
    }
  } // namespace detail

} // namespace yave