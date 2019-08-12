//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/data/lib/iterator.hpp>

#include <string>
#include <cstring>
#include <stdexcept>

namespace yave {

  /// (loose) UTF-8 string.
  /// \notes Currently string still allows to construct from char* string
  /// without narrow/UTF8 conversion, so user must provide properly encoded
  /// UTF-8 string for those constructors. In the futue, we can emit those
  /// narrow charactor constructors and use char8_t instead, but we'll still
  /// need narrow/UTF8 convertion helpers to support STL's char* interface
  /// (namely, `std::exception::what()`). At this point, we just leave some
  /// char8_t compatible interface and don't touch encoding.
  class string
  {
  public:
    using pointer                = char*;
    using const_pointer          = const char*;
    using size_type              = uint64_t;
    using value_type             = char;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using iterator               = yave::iterator<pointer, string>;
    using const_iterator         = yave::iterator<const_pointer, string>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    string(nullptr_t) = delete;

    /// Construct empty string.
    string()
      : m_size {0}
    {
      auto* buff = (char*)malloc(1);
      if (!buff)
        throw std::bad_alloc();
      buff[0] = '\0';
      m_ptr   = buff;
    }

    /// Construct from char pointer.
    /// \param str UTF-8 encoded C-style string.
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

    /// Construct from char8_t pointer
    /// \param UTF-8 string.
    string(const char8_t* str)
    {
      auto len   = std::strlen(reinterpret_cast<const char*>(str));
      auto* buff = (char*)std::malloc(len + 1);
      if (!buff)
        throw std::bad_alloc();
      std::copy(str, str + len + 1, buff);
      m_ptr  = buff;
      m_size = len;
    }

    /// Construct from std::string.
    /// \param str std::string which contains UTF-8 encoded string.
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

    /// Construct from std::u8string.
    /// \param str UTF-8 string.
    string(const std::basic_string<char8_t>& str)
    {
      auto len   = str.length();
      auto* buff = (char*)std::malloc(len + 1);
      if (!buff)
        throw std::bad_alloc();
      std::copy(str.c_str(), str.c_str() + len + 1, buff);
      m_ptr  = buff;
      m_size = len;
    }

    /// Copy constructor.
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

    /// Move constructor.
    string(string&& other) noexcept
    {
      m_ptr        = other.m_ptr;
      m_size       = other.m_size;
      other.m_ptr  = nullptr;
      other.m_size = 0;
    }

    /// Copy assignment operator.
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

    /// Move assignment operator.
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

    /// Destructor.
    ~string() noexcept
    {
      std::free(m_ptr);
    }

    /// Get C style string.
    [[nodiscard]] const char* c_str() const noexcept
    {
      return reinterpret_cast<const char*>(m_ptr);
    }

    /// Get char8_t string.
    [[nodiscard]] const char8_t* u8_str() const noexcept
    {
      return reinterpret_cast<const char8_t*>(m_ptr);
    }

    /// Get string length.
    [[nodiscard]] size_t length() const noexcept
    {
      return m_size;
    }

    [[nodiscard]] iterator begin() noexcept
    {
      return iterator(m_ptr);
    }

    [[nodiscard]] iterator end() noexcept
    {
      return iterator(m_ptr + m_size);
    }

    [[nodiscard]] const_iterator begin() const noexcept
    {
      return const_iterator(m_ptr);
    }

    [[nodiscard]] const_iterator end() const noexcept
    {
      return const_iterator(m_ptr);
    }

    [[nodiscard]] const_iterator cbegin() const noexcept
    {
      return begin();
    }

    [[nodiscard]] const_iterator cend() const noexcept
    {
      return end();
    }

  private:
    /// data
    char* m_ptr;
    /// size
    uint64_t m_size;
  };

  /// operator== for yave::string
  inline bool operator==(const string& lhs, const string& rhs)
  {
    return std::strcmp(lhs.c_str(), rhs.c_str()) == 0;
  }

  /// operator== for yave::string
  inline bool operator==(const char* lhs, const string& rhs)
  {
    return std::strcmp(lhs, rhs.c_str()) == 0;
  }

  /// operator== for yave::string
  inline bool operator==(const string& lhs, const char* rhs)
  {
    return std::strcmp(lhs.c_str(), rhs) == 0;
  }

} // namespace yave
