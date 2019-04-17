//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/exception.hpp>

namespace yave {

  // ------------------------------------------
  // Result errors

  namespace result_error {

    /// result error
    class result_error : public std::runtime_error
    {
    public:
      result_error(const char* msg = "result_error")
        : runtime_error(msg)
      {
      }
    };

    /// exception_result
    class exception_result : public result_error
    {
    public:
      exception_result(object_ptr<const Exception> e)
        : result_error("Exception detected while evaluation")
        , m_exception {std::move(e)}
      {
      }

      /// exception
      [[nodiscard]] const object_ptr<const Exception>& exception() const
      {
        return m_exception;
      }

    private:
      object_ptr<const Exception> m_exception;
    };

  } // namespace result_error

  // ------------------------------------------
  // conversion

  [[nodiscard]] inline object_ptr<const Exception>
    to_Exception(const result_error::exception_result& e)
  {
    // forward
    return e.exception();
  }

  [[nodiscard]] inline object_ptr<const Exception>
    to_Exception(const result_error::result_error& e)
  {
    return make_object<Exception>(
      make_object<String::String>(e.what()), make_object<String::String>(""));
  }

} // namespace yave