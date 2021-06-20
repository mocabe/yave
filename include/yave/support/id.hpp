//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>

#include <string>

namespace yave {

  /// Unique ID for object/nodes (64bit)
  struct uid
  {
    uint64_t data = 0;

    /// Generate random id.
    [[nodiscard]] static uid random_generate();
  };

  [[nodiscard]] bool operator<(const uid& lhs, const uid& rhs) noexcept;
  [[nodiscard]] bool operator>(const uid& lhs, const uid& rhs) noexcept;
  [[nodiscard]] bool operator<=(const uid& lhs, const uid& rhs) noexcept;
  [[nodiscard]] bool operator>=(const uid& lhs, const uid& rhs) noexcept;
  [[nodiscard]] bool operator==(const uid& lhs, const uid& rhs) noexcept;
  [[nodiscard]] bool operator!=(const uid& lhs, const uid& rhs) noexcept;

  /// Convert ID to string
  [[nodiscard]] std::string to_string(const uid& id);

} // namespace yave