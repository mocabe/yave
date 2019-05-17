//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/config.hpp>

#include <string>
#include <cstring>
#include <stdexcept>

namespace yave {

  /// UTF-8 string value
  class string
  {
    // TODO: support char8_t in C++20
  public:
    string(nullptr_t) = delete;

    /// Construct empty string
    string()
      : m_size {0}
    {
      auto* buff = (char*)malloc(1);
      if (!buff)
        throw std::bad_alloc();
      buff[0] = '\0';
      m_ptr   = buff;
    }

    /// Construct from char pointer
    string(const char* str)
    {
      auto len   = std::strlen(str);
      auto* buff = (char*)std::malloc(len + 1);
      if (!buff)
        throw std::bad_alloc();
      std::copy(str, str + len + 1, buff);
      m_ptr  = buff;
      m_size = len;
    }

    /// Construct from std::string
    string(const std::string& str)
    {
      auto len   = str.length();
      auto* buff = (char*)std::malloc(len + 1);
      if (!buff)
        throw std::bad_alloc();
      std::copy(str.c_str(), str.c_str() + len + 1, buff);
      m_ptr  = buff;
      m_size = len;
    }

    /// Copy constructor
    string(const string& other)
    {
      auto len   = other.length();
      auto* buff = (char*)std::malloc(len + 1);
      if (!buff)
        throw std::bad_alloc();
      std::copy(other.m_ptr, other.m_ptr + len + 1, buff);
      m_ptr  = buff;
      m_size = len;
    }

    /// Move constructor
    string(string&& other) noexcept
    {
      m_ptr        = other.m_ptr;
      m_size       = other.m_size;
      other.m_ptr  = nullptr;
      other.m_size = 0;
    }

    /// Copy assignment
    string& operator=(const string& other)
    {
      auto len   = other.length();
      auto* buff = (char*)std::realloc(m_ptr, len + 1);
      if (!buff)
        throw std::bad_alloc();
      std::copy(other.m_ptr, other.m_ptr + len + 1, buff);
      m_ptr  = buff;
      m_size = len;
      return *this;
    }

    /// Move assignment
    string& operator=(string&& other) noexcept
    {
      auto* tmp    = m_ptr;
      m_ptr        = other.m_ptr;
      m_size       = other.m_size;
      other.m_ptr  = nullptr;
      other.m_size = 0;
      std::free(tmp);
      return *this;
    }

    /// Destructor
    ~string() noexcept
    {
      std::free(m_ptr);
    }

    /// Get C style string
    [[nodiscard]] const char* c_str() const noexcept
    {
      return reinterpret_cast<const char*>(m_ptr);
    }

    /// Get string length
    [[nodiscard]] size_t length() const noexcept
    {
      return m_size;
    }

  private:
    /// data
    char* m_ptr;
    /// size
    uint64_t m_size;
  };

} // namespace yave
