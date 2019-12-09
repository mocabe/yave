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
    class result_error : public std::runtime_error
    {
    public:
      using std::runtime_error::runtime_error;

      result_error()
        : runtime_error("result_error")
      {
      }
    };

    /// invalid result value (i.e. null object)
    class null_result : public result_error
    {
    public:
      null_result()
        : result_error("Null result detected while evaluation")
      {
      }
    };

  } // namespace result_error

  [[nodiscard]] inline auto to_Exception(const result_error::result_error& e)
    -> object_ptr<Exception>
  {
    return make_object<Exception>(
      e.what(), make_object<ResultError>(result_error_type::unknown));
  }

  [[nodiscard]] inline auto to_Exception(const result_error::null_result& e)
    -> object_ptr<Exception>
  {
    return make_object<Exception>(
      e.what(), make_object<ResultError>(result_error_type::null_result));
  }

  // ------------------------------------------
  // Exception result

  /// exception_result
  class exception_result : public std::runtime_error
  {
  public:
    exception_result(object_ptr<const Exception> e)
      : runtime_error("Exception detected while evaluation")
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

  [[nodiscard]] inline auto to_Exception(const exception_result& e)
    -> object_ptr<const Exception>
  {
    // forward
    return e.exception();
  }

  [[nodiscard]] inline auto to_Exception(const std::exception& e)
    -> object_ptr<Exception>
  {
    return make_object<Exception>(
      e.what(), make_object<ResultError>(result_error_type::stdexcept));
  }

  [[nodiscard]] inline auto to_Exception()
  {
    return make_object<Exception>(
      "Unknown exception detected while evaluation",
      make_object<ResultError>(result_error_type::unknown));
  }

} // namespace yave

YAVE_DECL_TYPE(yave::ResultError, "83f108e0-a8f6-4881-bb7a-c1f0c10c36ef");