//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <string>

namespace yave {

  /// 128bit UUID
  struct uuid
  {
    unsigned char data[16];

    /// Generate random UUID.
    [[nodiscard]] static uuid random_generate();
  };

  [[nodiscard]] bool operator<(const uuid& lhs, const uuid& rhs) noexcept;
  [[nodiscard]] bool operator>(const uuid& lhs, const uuid& rhs) noexcept;
  [[nodiscard]] bool operator<=(const uuid& lhs, const uuid& rhs) noexcept;
  [[nodiscard]] bool operator>=(const uuid& lhs, const uuid& rhs) noexcept;
  [[nodiscard]] bool operator==(const uuid& lhs, const uuid& rhs) noexcept;
  [[nodiscard]] bool operator!=(const uuid& lhs, const uuid& rhs) noexcept;

  /// Convert UUID to string
  [[nodiscard]] std::string to_string(const uuid& id);

} // namespace yave