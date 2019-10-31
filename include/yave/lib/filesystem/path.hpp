//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/lib/string/string.hpp>
#include <yave/lib/filesystem/filesystem.hpp>

namespace yave::filesystem {

  /// Platform agnostic UTF-8 path string.
  /// Has minimum interface to get string representation, conversion to
  /// std::filesystem::path for real use.
  class path
  {
  public:
    path(const std::filesystem::path& path)
      : m_string {path.generic_u8string()}
    {
    }

    path()
      : path(std::filesystem::path())
    {
    }

    path(const char* str)
      : path(std::filesystem::path(str))
    {
    }

    path(const char8_t* str)
      : path(std::filesystem::u8path((const char*)str))
    {
    }

    path(const path&) = default;
    path(path&&)      = default;
    path& operator=(const path&) = default;
    path& operator=(path&&) = default;

    /// Conversion to std::filesystem::path
    [[nodiscard]] operator std::filesystem::path() const
    {
      return std::filesystem::u8path(m_string.c_str());
    }

    /// Get C style string
    [[nodiscard]] auto c_str() const noexcept -> const char*
    {
      return m_string.c_str();
    }

    /// Get char8_t string
    [[nodiscard]] auto u8_str() const noexcept -> const char8_t*
    {
      return m_string.u8_str();
    }

    /// Get string
    [[nodiscard]] auto string() const -> yave::string
    {
      return std::string(m_string);
    }

    friend inline bool operator==(const path& lhs, const path& rhs)
    {
      return lhs.m_string == rhs.m_string;
    }

    friend inline bool operator!=(const path& lhs, const path& rhs)
    {
      return !(lhs == rhs);
    }

  private:
    yave::string m_string;
  };

} // namespace yave