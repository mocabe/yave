//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <string>
#include <stdexcept>
#include <array>

namespace yave {

  /// 128bit UUID
  struct uuid
  {
    std::array<char, 16> data = {0};

    /// Generate random UUID.
    [[nodiscard]] static uuid random_generate();

    /// Generate UUID from string
    [[nodiscard]] static uuid from_string(std::string_view str);

    /// Generate UUID from constexpr char array
    /// ex) 707186a4-f043-4a08-8223-e03fe9c1b0ea\0
    [[nodiscard]] static consteval uuid from_char_array(const char (&str)[37]);
  };

  [[nodiscard]] bool operator<(const uuid& lhs, const uuid& rhs) noexcept;
  [[nodiscard]] bool operator>(const uuid& lhs, const uuid& rhs) noexcept;
  [[nodiscard]] bool operator<=(const uuid& lhs, const uuid& rhs) noexcept;
  [[nodiscard]] bool operator>=(const uuid& lhs, const uuid& rhs) noexcept;
  [[nodiscard]] bool operator==(const uuid& lhs, const uuid& rhs) noexcept;
  [[nodiscard]] bool operator!=(const uuid& lhs, const uuid& rhs) noexcept;

  /// Convert UUID to string
  [[nodiscard]] std::string to_string(const uuid& id);

  consteval uuid uuid::from_char_array(char const (&str)[37])
  {
    char hex[32] {};
    size_t hex_idx = 0;

    // for gcc constexpr bug workaround
    bool fail = false;

    // read hex
    for (auto&& c : str) {
      if (c == '-' || c == '\0') {
        continue;
      }
      if ('0' <= c && c <= '9') {
        hex[hex_idx] = static_cast<char>(c - '0');
        ++hex_idx;
        continue;
      }
      if ('a' <= c && c <= 'f') {
        hex[hex_idx] = static_cast<char>(c - 'a' + 10);
        ++hex_idx;
        continue;
      }
      if ('A' <= c && c <= 'F') {
        hex[hex_idx] = static_cast<char>(c - 'A' + 10);
        ++hex_idx;
        continue;
      }
      fail = true;
      break;
    }

    if (fail)
      throw;

    uuid ret {};

    // convert to value
    for (size_t i = 0; i < 16; ++i) {
      auto upper  = 2 * i;
      auto lower  = 2 * i + 1;
      ret.data[i] = static_cast<char>(hex[upper] * 16 + hex[lower]);
    }
    return ret;
  }

} // namespace yave