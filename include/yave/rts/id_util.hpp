//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>

#include <array>
#include <string>

namespace yave {

  // ------------------------------------------
  // 128bit UUID

  namespace detail {

    /// read UUID from constexpr char array
    [[nodiscard]] constexpr auto read_uuid_from_constexpr_string(
      char const (&str)[37]) -> std::array<char, 16>
    {
      // ex) 707186a4-f043-4a08-8223-e03fe9c1b0ea\0

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
        break; // failed to parse UUID
      }

      if (fail)
        throw;

      std::array<char, 16> ret {};

      // convert to value
      for (size_t i = 0; i < 16; ++i) {
        auto upper = 2 * i;
        auto lower = 2 * i + 1;
        ret[i]     = static_cast<char>(hex[upper] * 16 + hex[lower]);
      }
      return ret;
    }

    // Convert UUID to string.
    // full : n=16
    // short: n=4
    inline auto uuid_to_string_impl(const std::array<char, 16>& uuid, size_t n)
      -> std::string
    {
      std::string ret;

      ret.reserve(2 * n + 4);

      auto _toc = [](uint32_t v) -> char {
        if (v >= 10)
          return static_cast<char>('a' + (v - 10));
        else
          return static_cast<char>('0' + v);
      };

      for (size_t i = 0; i < std::min(n, uuid.size()); ++i) {

        if (i == 4 || i == 6 || i == 8 || i == 10)
          ret += '-';

        uint32_t lo = 0x0F & uuid[i];
        uint32_t hi = 0x0F & uuid[i] >> 4;

        ret += _toc(lo);
        ret += _toc(hi);
      }

      return ret;
    }

    /// Convert UUID to short string
    [[nodiscard]] inline auto uuid_to_string_short(
      const std::array<char, 16>& uuid) -> std::string
    {
      return uuid_to_string_impl(uuid, 4);
    }

    /// Convert full UUID to string
    [[nodiscard]] inline auto uuid_to_string_full(
      const std::array<char, 16>& uuid) -> std::string
    {
      return uuid_to_string_impl(uuid, 16);
    }

  } // namespace detail

  // ------------------------------------------
  // 64bit ID

  namespace detail {

    // Convert 64bit ID to string.
    // short: n=4
    // long: n=8
    inline auto id_to_string_impl(const std::array<uint8_t, 8>& id, size_t n)
      -> std::string
    {
      auto _toc = [](uint32_t i) -> char {
        if (i >= 10)
          return static_cast<char>('a' + (i - 10));
        else
          return static_cast<char>('0' + i);
      };

      std::string ret;

      ret.reserve(n * 2);

      for (size_t i = 0; i < std::min(n, id.size()); ++i) {
        uint32_t low = 0x0F & id[i];
        uint32_t hi  = 0x0F & id[i] >> 4;
        ret += _toc(low);
        ret += _toc(hi);
      }
      return ret;
    }

    /// Convert 64bit ID to short string.
    [[nodiscard]] inline auto id_to_string_short(
      const std::array<uint8_t, 8>& id) -> std::string
    {
      return id_to_string_impl(id, 4);
    }

    /// Convert full 64bit ID to string.
    [[nodiscard]] inline auto id_to_string_full(
      const std::array<uint8_t, 8>& id) -> std::string
    {
      return id_to_string_impl(id, 8);
    }

  } // namespace detail
} // namespace yave