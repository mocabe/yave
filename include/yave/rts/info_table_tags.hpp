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

  namespace detail {

    // Info table pointer tags.
    // Uses lowest 3 bits of info table pointer.
    enum class info_table_tags : uintptr_t
    {
      // h <---------------------> l
      //                        |<>|
      //                        tag (3bit)
      //                        ^^^^^^^^^^
      vanilla      = 0,
      exception    = 1,                  // Exception
      apply        = 2,                  // Apply
      extract_mask = 0x0000000000000007, // 0...0111
      clear_mask   = 0xFFFFFFFFFFFFFFF8, // 1...1000
    };

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

    // Exception

    [[nodiscard]] inline auto add_exception_tag(const object_info_table* info)
      -> const object_info_table*
    {
      return add_info_table_tag(info, info_table_tags::exception);
    }

    [[nodiscard]] inline bool has_exception_tag(const object_info_table* tagged)
    {
      return check_info_table_tag(tagged, info_table_tags::exception);
    }

    // Apply

    [[nodiscard]] inline const object_info_table* add_apply_tag(
      const object_info_table* info)
    {
      return add_info_table_tag(info, info_table_tags::apply);
    }

    [[nodiscard]] inline bool has_apply_tag(const object_info_table* tagged)
    {
      return check_info_table_tag(tagged, info_table_tags::apply);
    }

  } // namespace detail
} // namespace yave