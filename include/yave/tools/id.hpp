//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#include <string>

namespace yave {

  /// Unique ID for object/nodes
  struct uid
  {
    uint64_t data;

    [[nodiscard]] static uid random_generate();
  };

  /// operator<
  [[nodiscard]] bool operator<(const uid& lhs, const uid& rhs) noexcept;
  /// operator>
  [[nodiscard]] bool operator>(const uid& lhs, const uid& rhs) noexcept;
  /// operator<=
  [[nodiscard]] bool operator<=(const uid& lhs, const uid& rhs) noexcept;
  /// operator>=
  [[nodiscard]] bool operator>=(const uid& lhs, const uid& rhs) noexcept;
  /// operator==
  [[nodiscard]] bool operator==(const uid& lhs, const uid& rhs) noexcept;

  /// Convert ID to string
  [[nodiscard]] std::string to_string(const uid& id);

} // namespace yave