//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/value_cast.hpp>
#include <yave/rts/function.hpp>
#include <yave/rts/eval_error.hpp>
#include <yave/rts/result_error.hpp>

namespace yave {

  /// copy apply graph
  [[nodiscard]] inline auto
    copy_apply_graph(const object_ptr<const Object>& obj)
      -> object_ptr<const Object>
  {
    if (auto apply = value_cast_if<Apply>(obj)) {
      auto& apply_storage = _get_storage(*apply);
      // return cached value
      if (apply_storage.is_result()) {
        return apply_storage.get_result();
      }
      // create new apply
      return make_object<Apply>(
        copy_apply_graph(apply_storage.app()),
        copy_apply_graph(apply_storage.arg()));
    }
    return obj;
  }

  namespace detail {

    /// evaluate apply tree
    [[nodiscard]] inline auto eval_obj(const object_ptr<const Object>& obj)
      -> object_ptr<const Object>
    {
      // detect exception
      if (unlikely(has_exception_tag(obj)))
        throw result_error::exception_result(
          clear_pointer_tag(get_tagged_exception(obj)));

      // apply
      if (auto apply = value_cast_if<Apply>(obj)) {

        // alias: internal storage
        auto& apply_storage = _get_storage(*apply);

        // graph reduction
        if (apply_storage.is_result()) {
          return apply_storage.get_result();
        }

        // whnf
        auto app  = eval_obj(apply_storage.app());
        auto capp = static_cast<const Closure<>*>(app.get());

        // alias: argument
        const auto& arg = apply_storage.arg();

        /*
          These exceptions should not triggered on well-typed input. Just
          leaving it here to avoid catastrophic heap corruption when something
          went totally wrong.
        */
        if (unlikely(!has_arrow_type(app))) {
          throw eval_error::bad_apply();
        }
        if (unlikely(capp->arity == 0)) {
          throw eval_error::too_many_arguments();
        }

        // clone if it's not pap
        auto pap  = (capp->is_pap()) ? std::move(app) : clone(app);
        auto cpap = static_cast<const Closure<>*>(pap.get());

        // push argument
        auto arity       = --cpap->arity;
        cpap->arg(arity) = arg;

        // call code()
        if (unlikely(arity == 0)) {
          auto ret = eval_obj(cpap->call());
          // set cache
          apply_storage.set_result(ret);
          return ret;
        }
        return pap;
      }
      return obj;
    }
  } // namespace detail

  /// evaluate each apply node and replace with result
  template <class T>
  [[nodiscard]] auto eval(object_ptr<T> obj)
  {
    auto result = detail::eval_obj(std::move(obj));
    assert(result);

    // for gcc 7
    constexpr auto type = type_of(get_term<T>(), false_c);

    // run compile time type check
    if constexpr (!is_error_type(type)) {
      // Currently object_ptr<T> MUST have type T which has compatible memory
      // layout with actual object pointing to.
      // Since it's impossible to decide memory layout of closure types,
      // we convert it to ClosureProxy<...> which is essentially equal to to
      // Object. Type variables are also undecidable so we just convert
      // them to proxy.
      using To =
        std::add_const_t<typename decltype(guess_object_type(type))::type>;
      // cast to resutn type
      return static_object_cast<To>(result);
    } else {
      // fallback to object_ptr<>
      return result;
    }
  }

} // namespace yave