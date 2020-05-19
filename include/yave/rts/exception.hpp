//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/box.hpp>
#include <yave/rts/object_cast.hpp>

#include <yave/obj/string/string.hpp>

#include <exception>

namespace yave {

  /// exception base with String message container
  class exception : public std::exception
  {
  public:
    exception(const char8_t* str)
      : m_str {make_object<String>(str)}
    {
    }

    exception(const exception& other) noexcept
      : m_str {other.m_str}
    {
    }

    exception(exception&& other) noexcept
      : m_str {std::move(other.m_str)}
    {
    }

    /// std::exception::what()
    const char* what() const noexcept override
    {
      return m_str->c_str();
    }

    /// UTF-8 error message
    const char8_t* u8_what() const noexcept
    {
      return m_str->u8_str();
    }

  private:
    object_ptr<const String> m_str;
  };

  /// \brief Exception value
  ///
  /// Exception object holds a pointer to arbitary error value.
  /// This is useful to detect exception object with single type comparison.
  struct exception_object_value
  {
    template <class T>
    exception_object_value(
      object_ptr<const String> msg,
      object_ptr<T> err) noexcept
      : m_message {std::move(msg)}
      , m_error_value {std::move(err)}
    {
    }

    template <class T>
    exception_object_value(const char* msg, object_ptr<T> err)
      : exception_object_value(make_object<String>(msg), std::move(err))
    {
    }

    /// Get error message
    [[nodiscard]] auto message() const -> std::string
    {
      return m_message->c_str();
    }

    /// Get error value object
    [[nodiscard]] auto error() const -> object_ptr<const Object>
    {
      return m_error_value;
    }

  private:
    /// message
    object_ptr<const String> m_message;
    /// pointer to error value
    object_ptr<const Object> m_error_value;
  };

  /// Exception
  using Exception = Box<exception_object_value>;

  template <>
  struct object_type_traits<yave::Exception>
  {
    static constexpr auto info_table_tag = detail::info_table_tags::_1;
    static constexpr char name[]         = "yave::Exception";
    static constexpr char uuid[] = "1a30465c-ee14-473e-bcb9-5ef2462d933f";
  };

} // namespace yave
