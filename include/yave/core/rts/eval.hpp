//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/rts/value_cast.hpp>
#include <yave/core/rts/function.hpp>
#include <yave/core/rts/eval_error.hpp>
#include <yave/core/rts/result_error.hpp>

namespace yave {

  /// copy apply graph
  [[nodiscard]] inline auto
    copy_apply_graph(const object_ptr<const Object>& obj)
      -> object_ptr<const Object>
  {
    if (auto apply = value_cast_if<Apply>(obj)) {
      auto& apply_storage = _get_storage(*apply);
      // return cached value
      if (apply_storage.evaluated()) {
        return apply_storage.get_cache();
      }
      // create new apply
      return make_object<Apply>(
        copy_apply_graph(apply_storage.app()),
        copy_apply_graph(apply_storage.arg()));
    }
    return obj;
  }

  /// clear apply cache
  inline void clear_apply_cache(const object_ptr<const Object>& obj)
  {
    if (auto app = value_cast_if<Apply>(obj)) {
      auto& storage = _get_storage(*app);
      if (storage.evaluated())
        storage.clear_cache();
      clear_apply_cache(storage.app());
      clear_apply_cache(storage.arg());
    }
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
        if (apply_storage.evaluated()) {
          return apply_storage.get_cache();
        }

        // whnf
        auto app = eval_obj(apply_storage.app());

        // alias: argument
        const auto& arg = apply_storage.arg();
        // alias: app closure
        auto capp = static_cast<const Closure<>*>(app.get());

        /*
          These exceptions should not triggered on well-typed input. Just
          leaving it here to avoid catastrophic heap corruption when something
          went totally wrong.
        */
        if (unlikely(!has_arrow_type(app))) {
          throw eval_error::bad_apply();
        }
        if (unlikely(capp->arity() == 0)) {
          throw eval_error::too_many_arguments();
        }

        // clone closure and apply
        auto ret = [&] {
          // clone
          auto pap = clone(app);
          // alias: pap closure
          auto cpap = static_cast<const Closure<>*>(pap.get());

          // push argument
          auto arity       = --cpap->arity();
          cpap->arg(arity) = arg;

          // call code()
          if (unlikely(arity == 0)) {
            return eval_obj(cpap->code());
          }

          return pap;
        }();

        // set cache
        apply_storage.set_cache(ret);

        return ret;
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