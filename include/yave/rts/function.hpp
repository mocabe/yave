//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/closure.hpp>

#include <yave/rts/static_typing.hpp>
#include <yave/rts/dynamic_typing.hpp>
#include <yave/rts/apply.hpp>
#include <yave/rts/exception.hpp>
#include <yave/rts/bad_value_cast.hpp>
#include <yave/rts/result_error.hpp>
#include <yave/rts/type_error.hpp>
#include <yave/rts/value_cast.hpp>
#include <yave/rts/eval.hpp>

#include <yave/obj/string/string.hpp>
#include <yave/support/offset_of_member.hpp>

namespace yave {

  namespace detail {

    // ------------------------------------------
    // vtbl_code_func

    /// vtable function to call code()
    template <class T>
    auto vtbl_code_func(const Closure<>* _cthis) noexcept
      -> object_ptr<const Object>
    {
      auto _this = static_cast<const T*>(_cthis);

      try {

        // code()
        auto code_result = _this->code().value();

        if (unlikely(!code_result))
          throw result_error::null_result();

        if (unlikely(has_type<Exception>(code_result)))
          return code_result;

        // TODO: Since we know return type at compile time, we can directly
        // convert applications into PAP without loop by analyzing TApply
        // tree. Same on other return types; we can bypass some of runtime
        // type checks which may improve performance.
        auto eval_result = detail::eval_obj(std::move(code_result));

        assert(eval_result);
        assert(!has_type<Exception>(eval_result));

        // save result
        _this->_cache(eval_result);

        return eval_result;

        /* cast error */
      } catch (const bad_value_cast& e) {
        return make_exception(e);

        /* type error */
      } catch (const type_error::circular_constraint& e) {
        return make_exception(e);
      } catch (const type_error::type_missmatch& e) {
        return make_exception(e);
      } catch (const type_error::bad_type_check& e) {
        return make_exception(e);
      } catch (const type_error::type_error& e) {
        return make_exception(e);

        /* result error */
      } catch (const result_error::null_result& e) {
        return make_exception(e);
      } catch (const result_error::result_error& e) {
        return make_exception(e);

        /* exception result transfer */
      } catch (const exception_result& e) {
        return make_exception(e);

        /* std::exception */
      } catch (const std::exception& e) {
        return make_exception(e);

        /* unknown exception */
      } catch (...) {
        return make_exception();
      }
    }

    // ------------------------------------------
    // return type checking

    template <class T1, class T2>
    constexpr auto check_return_type(meta_type<T1> t1, meta_type<T2> t2)
    {
      if constexpr (!match(t2, t1).is_succ()) {
        static_assert(false_v<T1, T2>, "return type does not match.");
        using lhs = typename T1::_show;
        using rhs = typename T2::_show;
        static_assert(false_v<lhs, rhs>, "compile-time type check failed.");
      }
    }

    /// Return type checker
    template <class T>
    class return_type_checker
    {
      static constexpr auto return_type = type_of(get_term<T>());

    public:
      /// object_ptr<U>
      template <class U>
      return_type_checker(object_ptr<U> obj) noexcept
        : m_value {std::move(obj)}
      {
        // check return type
        if constexpr (!std::is_same_v<std::decay_t<U>, Exception>) {
          check_return_type(return_type, type_of(get_term<U>()));
        }
      }

      // deleted
      return_type_checker()          = delete;
      return_type_checker(nullptr_t) = delete;

      /// value
      auto value() && noexcept -> auto&&
      {
        return std::move(m_value);
      }

    private:
      object_ptr<const Object> m_value;
    };

  } // namespace detail

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
    static constexpr auto term =
      generalize_tm_varvalue(get_term<closure<Ts...>>());

    /// Ctor
    Function() noexcept
      : ClosureN<sizeof...(Ts) - 1> //
      {
        {Object {1, 0, info_table_initializer::get_info_table(), nullptr},
         sizeof...(Ts) - 1},
      }
    {
    }

    /// Ctor (static)
    Function(static_construct_t) noexcept
      : Function()
    {
      this->refcount = 0;
    }

    /// Copy ctor
    Function(const Function& other) noexcept
      : ClosureN<sizeof...(Ts) - 1> //
      {{
         Object {other},
         other.arity,
       },
       other.spine}
    {
    }

    /// Move ctor
    Function(Function&& other) noexcept
      : ClosureN<sizeof...(Ts) - 1> //
      {{
         Object {other},
         std::move(other.arity),
       },
       std::move(other.spine)}
    {
    }

    /// operator=
    Function& operator=(const Function& other) noexcept
    {
      arity = other.arity;
      spine = other.spine;
      return *this;
    }

    /// operator=
    Function& operator=(Function&& other) noexcept
    {
      arity = std::move(other.arity);
      spine = std::move(other.spine);
      return *this;
    }

  protected:
    /// argument proxy type
    template <size_t N>
    using argument_proxy_t = std::add_const_t<typename decltype(
      get_proxy_type(normalize_specifier(get<N>(tuple_c<Ts...>))))::type>;

    /// return type for code()
    using return_type =
      detail::return_type_checker<argument_proxy_t<sizeof...(Ts) - 1>>;

    /// get N'th argument thunk
    template <uint64_t N>
    [[nodiscard]] auto arg() const noexcept
    {
      using To = argument_proxy_t<N>;
      static_assert(std::is_standard_layout_v<To>);
      auto obj = ClosureN<sizeof...(Ts) - 1>::template nth_arg<N>();
      assert(obj);
      return static_object_cast<To>(std::move(obj));
    }

    /// evaluate N'th argument and take result
    template <uint64_t N>
    [[nodiscard]] auto eval_arg() const
    {
      // workaround: gcc 8.1
      return eval(this->template arg<N>());
    }

  public: /* customization points */
    /// default self-update function.
    void _cache(const object_ptr<const Object>& result) const noexcept
    {
      auto cthis = reinterpret_cast<const Closure<>*>(this);
      _get_storage(*cthis->vertebrae(0)).set_result(result);
    }

  private:
    // You Can't See Me!
    using base = ClosureN<sizeof...(Ts) - 1>;
    using base::arity;
    using base::n_args;
    using base::call;
    using base::vertebrae;
    using base::is_pap;
    using base::arg;
    using base::spine;
    using base::nth_arg;
    using base::get_info_table;
    using base::value;

  private:
    /// Closure info table initializer
    struct info_table_initializer
    {
      static constexpr auto get_info_table() -> const object_info_table*
      {
        return detail::add_info_table_tag(
          &info_table, detail::get_info_table_tag<Closure<>>());
      }

      /// static closure info
      alignas(64) inline static const closure_info_table info_table {
        {object_type<T>(),                              //
         sizeof(T),                                     //
         closure_type_traits<Function<T, Ts...>>::name, //
         detail::vtbl_destroy_func<T>,                  //
         detail::vtbl_clone_func<T>},                   //
        sizeof...(Ts) - 1,                              //
        detail::vtbl_code_func<T>};                     //
    };
  };

} // namespace yave
