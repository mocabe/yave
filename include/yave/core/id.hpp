//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>

#include <string>

namespace yave {

  namespace detail {

    /// random generator
    uint64_t tagged_id_random();

    /// id to_string
    std::string tagged_id_to_string(uint64_t id);

  } // namespace detail

  /// Tagged ID type
  template <class T>
  struct tagged_id
  {
    uint64_t data = 0;

    /// Tag type
    using tag_type = T;

    /// Generate random id.
    [[nodiscard]] static tagged_id random_generate();

    /// Comparison
    auto operator<=>(const tagged_id&) const = default;
  };

  template <class T>
  tagged_id<T> tagged_id<T>::random_generate()
  {
    return {detail::tagged_id_random()};
  }

  /// Convert ID to string
  template <class T>
  [[nodiscard]] std::string to_string(const tagged_id<T>& id)
  {
    return detail::tagged_id_to_string(id.data);
  }

  /// Unique ID for object/nodes (64bit)
  using uid = tagged_id<void>;

} // namespace yave