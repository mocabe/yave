//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/exception.hpp>

namespace yave {

  // ------------------------------------------
  // Result errors

  enum class result_error_type : uint64_t
  {
    unknown     = 0,
    null_result = 1, // return value is null
    stdexcept   = 2, // detected std::exception
  };

  struct result_error_object_value
  {
    result_error_type error_type;
  };

  /// ResultError
  using ResultError = Box<result_error_object_value>;

  namespace result_error {

    /// result_error
    class result_error : public exception_base
    {
    public:
      using exception_base::exception_base;

      result_error()
        : exception_base(u8"yave::result_error")
      {
      }
    };

    /// invalid result value (i.e. null object)
    class null_result : public result_error
    {
    public:
      null_result()
        : result_error(u8"Null result detected while evaluation")
      {
      }
    };

  } // namespace result_error

  [[nodiscard]] inline auto make_exception(const result_error::result_error& e)
    -> object_ptr<Exception>
  {
    return make_object<Exception>(
      e.message(), make_object<ResultError>(result_error_type::unknown));
  }

  [[nodiscard]] inline auto make_exception(const result_error::null_result& e)
    -> object_ptr<Exception>
  {
    return make_object<Exception>(
      e.message(), make_object<ResultError>(result_error_type::null_result));
  }

  // ------------------------------------------
  // Exception result

  /// exception_result
  class exception_result : public exception_base
  {
  public:
    exception_result(object_ptr<const Exception> e)
      : exception_base(u8"Exception detected while evaluation")
      , m_exception {std::move(e)}
    {
    }

    /// exception
    [[nodiscard]] auto exception() const -> const object_ptr<const Exception>&
    {
      return m_exception;
    }

  private:
    object_ptr<const Exception> m_exception;
  };

  // ------------------------------------------
  // conversion

  [[nodiscard]] inline auto make_exception(const exception_result& e)
    -> object_ptr<const Exception>
  {
    // forward
    return e.exception();
  }

  [[nodiscard]] inline auto make_exception(const std::exception& e)
    -> object_ptr<Exception>
  {
    /// TODO: check UTF-8?
    return make_object<Exception>(
      (const char8_t*)e.what(),
      make_object<ResultError>(result_error_type::stdexcept));
  }

  [[nodiscard]] inline auto make_exception()
  {
    return make_object<Exception>(
      u8"Unknown exception detected while evaluation",
      make_object<ResultError>(result_error_type::unknown));
  }

} // namespace yave

YAVE_DECL_TYPE(yave::ResultError, "83f108e0-a8f6-4881-bb7a-c1f0c10c36ef");