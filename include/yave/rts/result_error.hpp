//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/exception.hpp>

namespace yave {

  // ------------------------------------------
  // Result errors

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

} // namespace yave