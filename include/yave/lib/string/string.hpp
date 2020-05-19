//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/config/config.hpp>
#include <yave/lib/util/iterator.hpp>

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
    using pointer                = char8_t*;
    using const_pointer          = const char8_t*;
    using size_type              = uint64_t;
    using value_type             = char8_t;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using iterator               = yave::iterator<pointer, string>;
    using const_iterator         = yave::iterator<const_pointer, string>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    string(nullptr_t) = delete;

    /// Construct empty string.
    string()
    {
      m_ptr  = _alloc(1);
      m_size = 0;
    }

    /// Construct from char pointer.
    /// \param str UTF-8 encoded C-style string.
    explicit string(const char* str)
    {
      auto len  = std::strlen(str);
      auto buff = _alloc(len + 1);
      std::copy(str, str + len, buff);
      m_ptr  = buff;
      m_size = len;
    }

    /// Construct from char8_t pointer
    /// \param UTF-8 string.
    string(const char8_t* str)
    {
      auto len  = std::strlen(reinterpret_cast<const char*>(str));
      auto buff = _alloc(len + 1);
      std::copy(str, str + len, buff);
      m_ptr  = buff;
      m_size = len;
    }

    /// Construct from std::string.
    /// \param str std::string which contains UTF-8 encoded string.
    explicit string(const std::string& str)
    {
      auto len  = str.length();
      auto buff = _alloc(len + 1);
      std::copy(str.c_str(), str.c_str() + len, buff);
      m_ptr  = buff;
      m_size = len;
    }

    /// Construct from std::u8string.
    /// \param str UTF-8 string.
    string(const std::u8string& str)
    {
      auto len  = str.length();
      auto buff = _alloc(len + 1);
      std::copy(str.c_str(), str.c_str() + len, buff);
      m_ptr  = buff;
      m_size = len;
    }

    /// Copy constructor.
    string(const string& other)
    {
      if (this == std::addressof(other)) {
        m_ptr  = _alloc(1);
        m_size = 0;
        return;
      }
      auto len  = other.length();
      auto buff = _alloc(len + 1);
      std::copy(other.m_ptr, other.m_ptr + len, buff);
      m_ptr  = buff;
      m_size = len;
    }

    /// Move constructor.
    string(string&& other) noexcept
    {
      m_ptr  = _alloc(1);
      m_size = 0;
      swap(other);
    }

    /// Copy assignment operator.
    string& operator=(const string& other)
    {
      if (this == std::addressof(other)) {
        return *this;
      }
      auto tmp = other;
      swap(tmp);
      return *this;
    }

    /// Move assignment operator.
    string& operator=(string&& other) noexcept
    {
      string tmp = std::move(other);
      swap(tmp);
      return *this;
    }

    /// Destructor.
    ~string() noexcept
    {
      _dealloc(m_ptr, m_size + 1);
    }

    /// Conversion to std::string
    [[nodiscard]] operator std::string() const
    {
      return std::string(c_str());
    }

    /// Conversion to std::u8string
    [[nodiscard]] operator std::u8string() const
    {
      return std::u8string(u8_str());
    }

    /// Get C style string.
    [[nodiscard]] const char* c_str() const noexcept
    {
      return (const char*)m_ptr;
    }

    /// Get char8_t string.
    [[nodiscard]] const char8_t* u8_str() const noexcept
    {
      return m_ptr;
    }

    /// Get string length.
    [[nodiscard]] size_t length() const noexcept
    {
      return m_size;
    }

    /// begin
    [[nodiscard]] iterator begin() noexcept
    {
      return iterator(m_ptr);
    }

    /// end
    [[nodiscard]] iterator end() noexcept
    {
      return iterator(m_ptr + m_size);
    }

    /// begin
    [[nodiscard]] const_iterator begin() const noexcept
    {
      return const_iterator(m_ptr);
    }

    /// end
    [[nodiscard]] const_iterator end() const noexcept
    {
      return const_iterator(m_ptr);
    }

    /// begin
    [[nodiscard]] const_iterator cbegin() const noexcept
    {
      return begin();
    }

    /// end
    [[nodiscard]] const_iterator cend() const noexcept
    {
      return end();
    }

    /// data
    [[nodiscard]] const_pointer data() const noexcept
    {
      return m_ptr;
    }

    /// data
    [[nodiscard]] pointer data() noexcept
    {
      return m_ptr;
    }

    /// swap with other string
    void swap(string& other) noexcept
    {
      std::swap(m_ptr, other.m_ptr);
      std::swap(m_size, other.m_size);
    }

  private:
    /// 1 byte null string
    inline static char8_t _null = '\0';

  private:
    static char8_t* _alloc(size_t buff_size)
    {
      if (buff_size == 1)
        return &_null;

      auto buff           = new char8_t[buff_size];
      buff[buff_size - 1] = '\0';
      return buff;
    }

    static void _dealloc(const char8_t* ptr, size_t buff_size)
    {
      if (buff_size == 1)
        return;

      delete[] ptr;
    }

  private:
    /// data
    char8_t* m_ptr;
    /// size
    uint64_t m_size;
  };

  inline bool operator==(const string& lhs, const string& rhs)
  {
    return std::strcmp(lhs.c_str(), rhs.c_str()) == 0;
  }

  inline bool operator!=(const std::string& lhs, const std::string& rhs)
  {
    return !(lhs == rhs);
  }

  inline bool operator==(const char8_t* lhs, const string& rhs)
  {
    return std::strcmp((const char*)lhs, rhs.c_str()) == 0;
  }

  inline bool operator!=(const char8_t* lhs, const string& rhs)
  {
    return !(lhs == rhs);
  }

  inline bool operator==(const string& lhs, const char8_t* rhs)
  {
    return std::strcmp(lhs.c_str(), (const char*)rhs) == 0;
  }

  inline bool operator!=(const string& lhs, const char8_t* rhs)
  {
    return !(lhs == rhs);
  }

} // namespace yave
