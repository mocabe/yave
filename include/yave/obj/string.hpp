//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>

#include <string>
#include <cstring>
#include <string>

namespace yave {

  class string
  {
    // TODO: support char8_t in C++20
  public:
    string(nullptr_t) = delete;

    /// Construct empty string
    string()
    {
      m_ptr = new char[1] {'\0'};
    }

    /// Construct from char pointer
    string(const char* str)
    {
      size_t s  = std::strlen(str);
      auto buff = new char[s + 1];
      std::copy(str, str + s + 1, buff);
      m_ptr = buff;
    }

    /// Construct from std::string
    string(const std::string& str)
    {
      auto buff = new char[str.size() + 1];
      std::copy(str.c_str(), str.c_str() + str.size() + 1, buff);
      m_ptr = buff;
    }

    /// Copy constructor
    string(const string& other)
    {
      auto len  = std::strlen(other.c_str());
      auto buff = new char[len + 1];
      std::copy(other.m_ptr, other.m_ptr + len + 1, buff);
      m_ptr = buff;
    }

    /// Move constructor
    string(string&& other)
    {
      m_ptr       = other.m_ptr;
      other.m_ptr = nullptr;
    }

    /// Destructor
    ~string() noexcept
    {
      delete[] m_ptr;
    }

    /// Get C style string
    [[nodiscard]] const char* c_str() const noexcept
    {
      return reinterpret_cast<const char*>(m_ptr);
    }

  private:
    char* m_ptr;
  };

  namespace String {
    /// UTF-8 String object.
    /// Does not handle anything about other encodings. User must ensure
    /// input byte sequence is null(`0x00`)-terminated UTF-8 string.
    using String = Box<yave::string>;
  } // namespace String

  namespace literals {

    /// String object literal
    [[nodiscard]] inline object_ptr<String::String>
      operator"" _S(const char* str, size_t)
    {
      return make_object<String::String>(str);
    }

  } // namespace literals

} // namespace yave

// String
YAVE_DECL_TYPE(yave::String::String, "1198939c-c273-4875-a229-245abad7ef04");
