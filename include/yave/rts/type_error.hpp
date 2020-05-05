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
    unbounded_variable   = 1,
    circular_constraints = 2,
    type_missmatch       = 3,
    bad_type_check       = 4,
  };

  /// type_error_object_value
  struct type_error_object_value
  {
    /// type
    type_error_type error_type;

    // Active when:
    //  unbounded_variable   : var
    //  type_missmatch       : expected
    //  bad_type_check       : expected
    object_ptr<const Type> t1;

    // Active when:
    //  circular_constraints : var
    //  type_missmatch       : provided
    //  bad_type_check       : provided
    object_ptr<const Type> t2;
  };

  // TypeError
  using TypeError = Box<type_error_object_value>;

  namespace type_error {

    /// type_error
    class type_error : public std::logic_error
    {
    public:
      type_error(
        const char* msg                 = "type_error",
        object_ptr<const Object> source = nullptr)
        : logic_error(msg)
        , m_source {std::move(source)}
      {
      }

      /// has valid source?
      [[nodiscard]] bool has_source() const
      {
        return static_cast<bool>(m_source);
      }

      /// error source node
      [[nodiscard]] auto& source() const
      {
        return m_source;
      }

      /// error source node
      [[nodiscard]] auto& source()
      {
        return m_source;
      }

    private:
      object_ptr<const Object> m_source;
    };

    /// Invalid variable found
    class unbounded_variable : public type_error
    {
    public:
      unbounded_variable(
        object_ptr<const Type> var,
        object_ptr<const Object> src)
        : type_error("Unbounded variable detected", std::move(src))
        , m_var {std::move(var)}
      {
      }

      /// var
      [[nodiscard]] auto& var() const
      {
        return m_var;
      }

    private:
      object_ptr<const Type> m_var;
    };

    /// unification error(circular constraint)
    class circular_constraint : public type_error
    {
    public:
      circular_constraint(
        object_ptr<const Type> var,
        object_ptr<const Object> src)
        : type_error("Circular constraints", std::move(src))
        , m_var {std::move(var)}
      {
      }

      /// var
      [[nodiscard]] auto& var() const
      {
        return m_var;
      }

    private:
      object_ptr<const Type> m_var;
    };

    class unsolvable_constraints : public type_error
    {
    public:
      unsolvable_constraints(
        object_ptr<const Type> t1,
        object_ptr<const Type> t2,
        object_ptr<const Object> src)
        : type_error("Unsolvable constraits", std::move(src))
        , m_t1 {std::move(t1)}
        , m_t2 {std::move(t2)}
      {
      }

      /// t1
      [[nodiscard]] auto& t1() const
      {
        return m_t1;
      }

      /// t2
      [[nodiscard]] auto& t2() const
      {
        return m_t2;
      }

    private:
      /// t1
      object_ptr<const Type> m_t1;
      /// t2
      object_ptr<const Type> m_t2;
    };

    class kind_missmatch : public type_error
    {
    public:
      kind_missmatch(
        object_ptr<const Kind> expected,
        object_ptr<const Kind> provided,
        object_ptr<const Object> src)
        : type_error("Kind missmatch", std::move(src))
        , m_expected {std::move(expected)}
        , m_provided {std::move(provided)}
      {
      }

      /// expected
      [[nodiscard]] auto& expected() const
      {
        return m_expected;
      }

      /// provided
      [[nodiscard]] auto& provided() const
      {
        return m_provided;
      }

    private:
      /// t1
      object_ptr<const Kind> m_expected;
      /// t2
      object_ptr<const Kind> m_provided;
    };

    /// unification error(missmatch)
    class type_missmatch : public type_error
    {
    public:
      type_missmatch(
        object_ptr<const Type> expected,
        object_ptr<const Type> provided,
        object_ptr<const Object> src)
        : type_error("Type missmatch", std::move(src))
        , m_expected {std::move(expected)}
        , m_provided {std::move(provided)}
      {
      }

      /// expected
      [[nodiscard]] auto& expected() const
      {
        return m_expected;
      }

      /// provided
      [[nodiscard]] auto& provided() const
      {
        return m_provided;
      }

    private:
      /// t1
      object_ptr<const Type> m_expected;
      /// t2
      object_ptr<const Type> m_provided;
    };

    /// bad type check
    class bad_type_check : public type_error
    {
    public:
      bad_type_check(
        object_ptr<const Type> expected,
        object_ptr<const Type> provided,
        object_ptr<const Object> src)
        : type_error("type_error: Runtime type check failed", std::move(src))
        , m_expected {std::move(expected)}
        , m_provided {std::move(provided)}
      {
      }

      /// expected
      [[nodiscard]] auto& expected() const
      {
        return m_expected;
      }

      /// result
      [[nodiscard]] auto& provided() const
      {
        return m_provided;
      }

    private:
      /// expected type
      object_ptr<const Type> m_expected;
      /// result type
      object_ptr<const Type> m_provided;
    };

    /// no valid overloading
    class no_valid_overloading : public type_error
    {
    public:
      no_valid_overloading(object_ptr<const Object> src)
        : type_error("type_error: No valid overloading", std::move(src))
      {
      }
    };

  } // namespace type_error

  // ------------------------------------------
  // Type errors

  [[nodiscard]] inline auto make_exception(const type_error::type_error& e)
    -> object_ptr<Exception>
  {
    return make_object<Exception>(
      e.what(),
      make_object<TypeError>(
        type_error_type::unknown,
        object_type<Undefined>(),
        object_type<Undefined>()));
  }

  [[nodiscard]] inline auto make_exception(
    const type_error::unbounded_variable& e) -> object_ptr<Exception>
  {
    return make_object<Exception>(
      e.what(),
      make_object<TypeError>(
        type_error_type::unbounded_variable,
        e.var(),
        object_type<Undefined>()));
  }

  [[nodiscard]] inline auto make_exception(
    const type_error::circular_constraint& e) -> object_ptr<Exception>
  {
    return make_object<Exception>(
      e.what(),
      make_object<TypeError>(
        type_error_type::circular_constraints,
        object_type<Undefined>(),
        e.var()));
  }

  [[nodiscard]] inline auto make_exception(const type_error::type_missmatch& e)
    -> object_ptr<Exception>
  {
    return make_object<Exception>(
      e.what(),
      make_object<TypeError>(
        type_error_type::type_missmatch, e.expected(), e.provided()));
  }

  [[nodiscard]] inline auto make_exception(const type_error::bad_type_check& e)
    -> object_ptr<Exception>
  {
    return make_object<Exception>(
      e.what(),
      make_object<TypeError>(
        type_error_type::bad_type_check, e.expected(), e.provided()));
  }

} // namespace yave

// TypeError
YAVE_DECL_TYPE(yave::TypeError, "fd63bfe8-955b-40b3-a506-29b4e628ed1b");