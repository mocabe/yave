//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/exception.hpp>

namespace yave {

  // ------------------------------------------
  // Type errors

  enum class type_error_type : uint64_t
  {
    unknown              = 0,
    circular_constraints = 1,
    type_missmatch       = 2,
    bad_type_check       = 3,
  };

  /// type_error_object_value
  struct type_error_object_value
  {
    /// type
    type_error_type error_type;

    // not null(undefined) when:
    //  type_missmatch
    //  bad_type_check
    object_ptr<const Type> expected;

    // not null(undefined) when:
    //  circular_constraints
    //  type_missmatch
    //  bad_type_check
    object_ptr<const Type> provided;
  };

  // TypeError
  using TypeError = Box<type_error_object_value>;

  namespace type_error {

    /// type_error
    class type_error : public std::logic_error
    {
    public:
      /// Ctor string
      template <class T>
      explicit type_error(const std::string& what, object_ptr<T> src)
        : std::logic_error(what)
        , m_src {std::move(src)}
      {
      }

      template <class T>
      /// Ctor const char*
      explicit type_error(const char* what, object_ptr<T> src)
        : std::logic_error(what)
        , m_src {std::move(src)}
      {
      }

      /// get source node
      [[nodiscard]] const object_ptr<const Object>& src() const
      {
        return m_src;
      }

    private:
      /// source node
      object_ptr<const Object> m_src;
    };

    /// unification error(circular constraint)
    class circular_constraint : public type_error
    {
    public:
      circular_constraint(
        object_ptr<const Object> src,
        object_ptr<const Type> var)
        : type_error("Circular constraints", std::move(src))
        , m_var {std::move(var)}
      {
      }

      /// var
      [[nodiscard]] const object_ptr<const Type>& var() const
      {
        return m_var;
      }

    private:
      object_ptr<const Type> m_var;
    };

    /// unification error(missmatch)
    class type_missmatch : public type_error
    {
    public:
      type_missmatch(
        object_ptr<const Object> src,
        object_ptr<const Type> t1,
        object_ptr<const Type> t2)
        : type_error("Type missmatch", std::move(src))
        , m_t1 {std::move(t1)}
        , m_t2 {std::move(t2)}
      {
      }

      /// t1
      [[nodiscard]] const object_ptr<const Type>& t1() const
      {
        return m_t1;
      }

      /// t2
      [[nodiscard]] const object_ptr<const Type>& t2() const
      {
        return m_t2;
      }

    private:
      /// t1
      object_ptr<const Type> m_t1;
      /// t2
      object_ptr<const Type> m_t2;
    };

    /// bad type check
    class bad_type_check : public type_error
    {
    public:
      bad_type_check(
        object_ptr<const Type> expected,
        object_ptr<const Type> result,
        object_ptr<const Object> obj)
        : type_error(
            "type_error: check_type failed. Result type is invalid",
            std::move(obj))
        , m_expected {std::move(expected)}
        , m_result {std::move(result)}
      {
      }

      /// expected
      [[nodiscard]] const object_ptr<const Type>& expected() const
      {
        return m_expected;
      }

      /// result
      [[nodiscard]] const object_ptr<const Type>& result() const
      {
        return m_result;
      }

    private:
      /// expected type
      object_ptr<const Type> m_expected;
      /// result type
      object_ptr<const Type> m_result;
    };

  } // namespace type_error

  // ------------------------------------------
  // Type errors

  [[nodiscard]] inline object_ptr<Exception>
    to_Exception(const type_error::type_error& e)
  {
    return make_object<Exception>(
      e.what(),
      make_object<TypeError>(type_error_type::unknown, nullptr, nullptr));
  }

  [[nodiscard]] inline object_ptr<Exception>
    to_Exception(const type_error::circular_constraint& e)
  {
    return make_object<Exception>(
      e.what(),
      make_object<TypeError>(
        type_error_type::circular_constraints, nullptr, e.var()));
  }

  [[nodiscard]] inline object_ptr<Exception>
    to_Exception(const type_error::type_missmatch& e)
  {
    return make_object<Exception>(
      e.what(),
      make_object<TypeError>(type_error_type::type_missmatch, e.t1(), e.t2()));
  }

  [[nodiscard]] inline object_ptr<Exception>
    to_Exception(const type_error::bad_type_check& e)
  {
    return make_object<Exception>(
      e.what(),
      make_object<TypeError>(
        type_error_type::bad_type_check, e.expected(), e.result()));
  }

} // namespace yave

// TypeError
YAVE_DECL_TYPE(yave::TypeError, "fd63bfe8-955b-40b3-a506-29b4e628ed1b");