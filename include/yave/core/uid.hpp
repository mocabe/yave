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
    uint64_t uid_random();

    /// id to_string
    std::string uid_to_string(uint64_t id);

  } // namespace detail

  /// Tagged ID type
  template <class T>
  struct uidT
  {
    uint64_t data = 0;

    /// Tag type
    using tag_type = T;

    /// Generate random id.
    [[nodiscard]] static uidT random_generate();

    /// Comparison
    [[nodiscard]] auto operator<=>(const uidT&) const = default;
  };

  template <class T>
  uidT<T> uidT<T>::random_generate()
  {
    return {detail::uid_random()};
  }

  /// Convert ID to string
  template <class T>
  [[nodiscard]] std::string to_string(const uidT<T>& id)
  {
    return detail::uid_to_string(id.data);
  }

  /// Unique ID for object/nodes (64bit)
  using uid = uidT<void>;

} // namespace yave