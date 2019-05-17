//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/rts/exception.hpp>

namespace yave {

  // ------------------------------------------
  // Eval errors

  /// eval_error_type
  enum class eval_error_type : uint64_t
  {
    unknown           = 0,
    bad_fix           = 1,
    bad_apply         = 2,
    too_may_arguments = 3,
  };

  /// eval_error_value
  struct eval_error_object_value
  {
    /// error type
    eval_error_type error_type;
  };

  /// EvalError
  using EvalError = Box<eval_error_object_value>;

  namespace eval_error {

    /// evaluation error
    class eval_error : public std::runtime_error
    {
    public:
      /// Ctor
      explicit eval_error(const char* msg = "eval_error")
        : std::runtime_error(msg)
      {
      }
    };

    /// bad apply for fix
    class bad_fix : public eval_error
    {
    public:
      bad_fix()
        : eval_error("Invalid use of fix operator")
      {
      }
    };

    /// bad apply (apply for value)
    class bad_apply : public eval_error
    {
    public:
      bad_apply()
        : eval_error("Invalid application")
      {
      }
    };

    /// too many arguments
    class too_many_arguments : public eval_error
    {
    public:
      too_many_arguments()
        : eval_error("Invalid number of arguments")
      {
      }
    };

  } // namespace eval_error

  // ------------------------------------------
  // conversion

  [[nodiscard]] inline object_ptr<Exception>
    to_Exception(const eval_error::eval_error& e)
  {
    return make_object<Exception>(
      e.what(), make_object<EvalError>(eval_error_type::unknown));
  }

  [[nodiscard]] inline object_ptr<Exception>
    to_Exception(const eval_error::bad_fix& e)
  {
    return make_object<Exception>(
      e.what(), make_object<EvalError>(eval_error_type::bad_fix));
  }

  [[nodiscard]] inline object_ptr<Exception>
    to_Exception(const eval_error::bad_apply& e)
  {
    return make_object<Exception>(
      e.what(), make_object<EvalError>(eval_error_type::bad_apply));
  }

  [[nodiscard]] inline object_ptr<Exception>
    to_Exception(const eval_error::too_many_arguments& e)
  {
    return make_object<Exception>(
      e.what(), make_object<EvalError>(eval_error_type::too_may_arguments));
  }

} // namespace yave

// EvalError
YAVE_DECL_TYPE(yave::EvalError, "5078fc13-127f-46ac-b95a-afa34e8e86e2");