//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/support/offset_of_member.hpp>

#include <yave/rts/static_typing.hpp>
#include <yave/rts/dynamic_typing.hpp>
#include <yave/rts/apply.hpp>
#include <yave/rts/exception.hpp>
#include <yave/rts/eval_error.hpp>
#include <yave/rts/bad_value_cast.hpp>
#include <yave/rts/result_error.hpp>
#include <yave/rts/type_error.hpp>
#include <yave/rts/value_cast.hpp>

#include <yave/obj/string/string.hpp>

namespace yave {

  // ------------------------------------------
  // Closure

  // forward decl
  template <uint64_t N>
  struct ClosureN;

  template <class Closure1 = ClosureN<1>>
  struct Closure;

  /// Info table for closure
  struct closure_info_table : object_info_table
  {
    /// Number of arguments
    const uint64_t n_args;
    /// vtable for code
    object_ptr<const Object> (*code)(const Closure<>*) noexcept;
  };

  /// Trait class for Object::obj_name for closure objects.
  template <class T>
  struct closure_type_traits
  {
    /// Object::obj_name can be used for ID of closures.
    static constexpr const char name[] = "00000000-0000-0000-0000-000000000000";
  };

  template <class Closure1>
  struct Closure : Object
  {
    /// Arity of this closure.
    mutable uint64_t arity;

    /// Get number of args
    auto n_args() const noexcept
    {
      return static_cast<const closure_info_table*>(info_table)->n_args;
    }

    /// PAP?
    bool is_pap() const noexcept
    {
      return n_args() != arity;
    }

    /// get nth argument
    auto& arg(uint64_t n) const noexcept
    {
      using arg_type = typename decltype(Closure1::args)::value_type;
      // offset to first element of argument buffer
      constexpr uint64_t offset = offset_of_member(&Closure1::args);
      static_assert(offset % sizeof(arg_type) == 0);
      // manually calc offset to avoid UB
      return ((arg_type*)this)[offset / sizeof(arg_type) + n];
    }

  public: /* can modify mutable members */
    /// Execute core with vtable function
    auto call() const noexcept
    {
      return static_cast<const closure_info_table*>(info_table)->code(this);
    }
  };

  /** \brief ClosureN
   * Contains static size array for incoming arguments.
   * Arguments will be filled from back to front.
   * So, first argument of closure will be LAST element of array for arguments.
   * If all arguments are passed, arity becomes zero and the closure is ready to
   * execute code.
   */
  template <uint64_t N>
  struct ClosureN : Closure<>
  {
    /// get raw arg
    template <uint64_t Arg>
    auto& nth_arg() const noexcept
    {
      static_assert(Arg < N, "Invalid index of argument");
      return args[N - Arg - 1];
    }

    /// args
    mutable std::array<object_ptr<const Object>, N> args = {};
  };

  // ------------------------------------------
  // vtbl_code_func

  /// vrtable function to call code()
  template <class T>
  object_ptr<const Object> vtbl_code_func(const Closure<>* _this) noexcept
  {
    auto ret = [&]() -> object_ptr<const Object> {
      try {
        auto r = (static_cast<const T*>(_this)->exception_handler()).value();
        assert(r);
        return r;

        // bad_value_cast
      } catch (const bad_value_cast& e) {
        return add_exception_tag(to_Exception(e));

        // type_error
      } catch (const type_error::circular_constraint& e) {
        return add_exception_tag(to_Exception(e));
      } catch (const type_error::type_missmatch& e) {
        return add_exception_tag(to_Exception(e));
      } catch (const type_error::bad_type_check& e) {
        return add_exception_tag(to_Exception(e));
      } catch (const type_error::type_error& e) {
        return add_exception_tag(to_Exception(e));

        // result_error
      } catch (const result_error::exception_result& e) {
        return add_exception_tag(to_Exception(e));
      } catch (const result_error::result_error& e) {
        return add_exception_tag(to_Exception(e));

        // eval_error
      } catch (const eval_error::bad_fix& e) {
        return add_exception_tag(to_Exception(e));
      } catch (const eval_error::bad_apply& e) {
        return add_exception_tag(to_Exception(e));
      } catch (const eval_error::too_many_arguments& e) {
        return add_exception_tag(to_Exception(e));
      } catch (const eval_error::eval_error& e) {
        return add_exception_tag(to_Exception(e));

        // std::exception
      } catch (const std::exception& e) {
        return add_exception_tag(to_Exception(e));

        // unknown
      } catch (...) {
        return add_exception_tag(make_object<Exception>(
          make_object<String>("Unknown exception thrown while evaluation"),
          object_ptr(nullptr)));
      }
    }();

    // vtbl_code_func should not return Undefined value
    assert(ret);
    // exception object retuned fron vtbl_code_func should have pointer tag
    if (!has_exception_tag(ret))
      assert(!value_cast_if<Exception>(ret));

    return ret;
  }

  // ------------------------------------------
  // return type checking

  template <class T1, class T2>
  constexpr auto check_return_type(meta_type<T1> t1, meta_type<T2> t2)
  {
    if constexpr (t1 != t2) {
      static_assert(false_v<T1>, "return type does not match.");
      using _t1 = typename T1::_print_expected;
      using _t2 = typename T2::_print_provided;
      static_assert(false_v<_t1, _t2>, "compile-time type check failed.");
    }
  }

  /// Return type checker
  template <class T>
  class return_type_checker
  {
    static constexpr auto return_type = type_of(get_term<T>());

  public:
    /// object_ptr<U>&&
    template <class U>
    return_type_checker(object_ptr<U> obj) noexcept
      : m_value {std::move(obj)}
    {
      // check return type
      check_return_type(return_type, type_of(get_term<U>()));
    }

    /// U*
    template <class U>
    return_type_checker(U* ptr) noexcept
      : m_value(ptr)
    {
      // check return type
      check_return_type(return_type, type_of(get_term<U>()));
    }

    /// copy
    return_type_checker(const return_type_checker& other) noexcept
      : m_value {other.m_value}
    {
    }

    /// move
    return_type_checker(return_type_checker&& other) noexcept
      : m_value {std::move(other.m_value)}
    {
    }

    // deleted
    return_type_checker()          = delete;
    return_type_checker(nullptr_t) = delete;

    /// value
    auto&& value() && noexcept
    {
      return std::move(m_value);
    }

  private:
    object_ptr<const Object> m_value;
  };

  /// Return type checker
  template <class T>
  class exception_handler_return_type_checker
  {
  public:
    /// return_type ctor
    exception_handler_return_type_checker(return_type_checker<T> e) noexcept
      : m_value {std::move(e).value()}
    {
    }

    /// Exception ctor
    template <class U>
    exception_handler_return_type_checker(object_ptr<U> e) noexcept
      : m_value {object_ptr<const Exception>(std::move(e))}
    {
      m_value = add_exception_tag(std::move(m_value));
    }

    /// Exception ctor
    exception_handler_return_type_checker(const Exception* e) noexcept
      : exception_handler_return_type_checker(object_ptr(e))
    {
      m_value = add_exception_tag(std::move(m_value));
    }

    // deleted
    exception_handler_return_type_checker()          = delete;
    exception_handler_return_type_checker(nullptr_t) = delete;
    exception_handler_return_type_checker(
      const exception_handler_return_type_checker&) = delete;
    exception_handler_return_type_checker(
      exception_handler_return_type_checker&&) = delete;

    /// value
    auto&& value() && noexcept
    {
      return std::move(m_value);
    }

  private:
    object_ptr<const Object> m_value;
  };

  // ------------------------------------------
  // Function

  /// CRTP utility to create closure type.
  template <class T, class... Ts>
  struct Function : ClosureN<sizeof...(Ts) - 1>
  {
    static_assert(
      sizeof...(Ts) > 1,
      "Closure should have argument and return type");

    /// term
    static constexpr auto term = get_term<closure<Ts...>>();

    /// Ctor
    Function() noexcept
      : ClosureN<sizeof...(Ts) - 1> {
          {{static_cast<const object_info_table*>(
             &info_table_initializer::info_table)},
           sizeof...(Ts) - 1},
        }
    {
    }

    /// Copy ctor
    Function(const Function& other) noexcept
      : ClosureN<sizeof...(Ts) - 1> {{
                                       {static_cast<const object_info_table*>(
                                         &info_table_initializer::info_table)},
                                       other.arity,
                                     },
                                     other.args}
    {
    }

    /// Move ctor
    Function(Function&& other) noexcept
      : ClosureN<sizeof...(Ts) - 1> {{
                                       {static_cast<const object_info_table*>(
                                         &info_table_initializer::info_table)},
                                       std::move(other.arity),
                                     },
                                     std::move(other.args)}
    {
    }

    /// operator=
    Function& operator=(const Function& other) noexcept
    {
      arity = other.arity;
      args  = other.args;
      return *this;
    }

    /// operator=
    Function& operator=(Function&& other) noexcept
    {
      arity = std::move(other.arity);
      args  = std::move(other.args);
      return *this;
    }

  protected:
    /// argument proxy type
    template <size_t N>
    using argument_proxy_t =
      std::add_const_t<typename decltype(get_argument_proxy_type(
        normalize_specifier(get<N>(tuple_c<Ts...>))))::type>;

    /// return type for code()
    using return_type =
      return_type_checker<argument_proxy_t<sizeof...(Ts) - 1>>;

    /// return type for exception_handler()
    using exception_handler_return_type = exception_handler_return_type_checker<
      argument_proxy_t<sizeof...(Ts) - 1>>;

    /// get N'th argument thunk
    template <uint64_t N>
    [[nodiscard]] auto arg() const noexcept
    {
      using To = argument_proxy_t<N>;
      static_assert(std::is_standard_layout_v<To>);
      auto obj = ClosureN<sizeof...(Ts) - 1>::template nth_arg<N>();
      assert(obj);
      return static_object_cast<To>(obj);
    }

    /// evaluate N'th argument and take result
    template <uint64_t N>
    [[nodiscard]] auto eval_arg() const
    {
      // workaround: gcc 8.1
      return eval(this->template arg<N>());
    }

  public:
    /// default exception handler
    exception_handler_return_type exception_handler() const
    {
      return static_cast<const T*>(this)->code();
    }

  private:
    // You Can't See Me!
    using base = ClosureN<sizeof...(Ts) - 1>;
    using base::arity;
    using base::n_args;
    using base::call;
    using base::arg;
    using base::args;
    using base::nth_arg;

    /// check signature of code()
    void check_code()
    {
      static_assert(
        std::is_same_v<return_type, decltype(std::declval<const T>().code())>,
        " `return_type code() const` was not found.");
    }

    // check_code
    using concept_check_code = concept_checker<&Function::check_code>;

  private:
    /// Closure info table initializer
    struct info_table_initializer
    {
      /// static closure info
      alignas(64) inline static const closure_info_table info_table {
        {object_type<T>(),                              //
         sizeof(T),                                     //
         closure_type_traits<Function<T, Ts...>>::name, //
         vtbl_destroy_func<T>,                          //
         vtbl_clone_func<T>},                           //
        sizeof...(Ts) - 1,                              //
        vtbl_code_func<T>};                             //
    };
  };

} // namespace yave
