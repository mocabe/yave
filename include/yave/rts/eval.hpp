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

    [[nodiscard]] inline auto build_spine_stack(
      const object_ptr<const Object>& obj,
      std::vector<object_ptr<const Apply>>& stack) -> object_ptr<const Object>
    {
      auto next = obj;
      for (;;) {
        if (auto apply = value_cast_if<Apply>(next)) {

          auto& apply_storage = _get_storage(*apply);

          // graph reduction
          if (apply_storage.is_result()) {
            // assume Exception is not cached
            next = apply_storage.get_result();
            continue;
          }

          if (stack.empty())
            stack.reserve(8);

          // push apply node itself (not argument!)
          stack.push_back(apply);

          next = apply_storage.app();
          continue;
        }
        // bottom
        return next;
      }
    };

    [[nodiscard]] inline auto eval_spine(
      const object_ptr<const Object>& obj,
      std::vector<object_ptr<const Apply>>& stack) -> object_ptr<const Object>
    {
      // detect exception
      if (unlikely(has_exception_tag(obj)))
        throw result_error::exception_result(
          clear_pointer_tag(get_tagged_exception(obj)));

      if (auto apply = value_cast_if<Apply>(obj)) {

        // build stack and get bottom closure
        auto bottom  = build_spine_stack(obj, stack);
        auto cbottom = reinterpret_cast<const Closure<>*>(bottom.get());

        assert(has_arrow_type(bottom));

        // not enough arguments!
        if (stack.size() < cbottom->n_args()) {
          // return current root
          return obj;
        }

        // clone closure when it possibly have mutable members
        auto fun  = clone(bottom);
        auto cfun = reinterpret_cast<const Closure<>*>(fun.get());

        assert(has_arrow_type(fun));

        // base of stack
        auto base_iter = stack.end() - cfun->n_args();

        // dump stack into closure
        for (size_t i = 0; i < cfun->n_args(); ++i) {
          cfun->vertebrae(i) = std::move(*(base_iter + i));
        }

        // call code
        auto result = cfun->call();

        // unwind stack
        stack.erase(base_iter, stack.end());

        return eval_spine(std::move(result), stack);
      }
      return obj;
    }

    /// evaluate apply tree
    [[nodiscard]] inline auto eval_obj(const object_ptr<const Object>& obj)
      -> object_ptr<const Object>
    {
      std::vector<object_ptr<const Apply>> stack;
      return eval_spine(obj, stack);
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