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
  [[nodiscard]] inline auto copy_apply_graph(
    const object_ptr<const Object>& obj) -> object_ptr<const Object>
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
      object_ptr<const Object> obj,
      std::vector<object_ptr<const Apply>>& stack) -> object_ptr<const Object>
    {
      // we, hopeless C++ programmers, manually optimize tailcalls because
      // compilers sometimes not smart enough to do that for us.

      auto next = std::move(obj);

      for (;;) {

        if (auto apply = value_cast_if<Apply>(next)) {

          auto& apply_storage = _get_storage(*apply);

          // graph reduction
          if (apply_storage.is_result()) {
            // assume Exception is not cached
            next = apply_storage.get_result();
            continue;
          }

          // push vertebrae
          stack.push_back(std::move(apply));

          next = apply_storage.app();
          continue;
        }
        // bottom
        return next;
      }
    }

    [[nodiscard]] inline auto eval_spine(object_ptr<const Object> obj)
      -> object_ptr<const Object>
    {
      if (auto apply = value_cast_if<Apply>(obj)) {

        auto& apply_storage = _get_storage(*apply);

        // when we don't have any arguments to apply in stack and apply node
        // has already evaluated, we can directly return cached result. if we
        // can guarantee cached results are only PAP or values, we actually
        // don't need to call eval_spine() on these results.
        if (apply_storage.is_result())
          return apply_storage.get_result();

        // spine stack
        std::vector<object_ptr<const Apply>> stack;

        // build stack and get bottom closure
        stack.push_back(std::move(apply));
        auto bottom = build_spine_stack(apply_storage.app(), stack);

        for (;;) {

          if (!has_arrow_type(bottom))
            throw eval_error::bad_apply();

          // clone bottom closure
          auto fun   = clone(bottom);
          auto cfun  = reinterpret_cast<const Closure<>*>(fun.get());
          auto arity = cfun->arity;
          auto size  = stack.size();

          auto asmin     = std::min(arity, size);
          auto base_iter = stack.end() - asmin;

          // dump to local stack
          for (size_t i = 0; i < asmin; ++i) {
            cfun->vertebrae(arity - asmin + i) = std::move(*(base_iter + i));
          }
          cfun->arity -= asmin;

          // not enough arguments; return PAP
          if (asmin != arity)
            return fun;

          // call code
          auto result = cfun->call();

          // unwind stack consumed
          stack.erase(base_iter, stack.end());

          // detect exception
          if (unlikely(has_exception_tag(result)))
            throw result_error::exception_result(
              clear_pointer_tag(get_tagged_exception(result)));

          // completed
          if (stack.empty())
            return result;

          // loop
          bottom = std::move(result);
          continue;
        }
      }
      return obj;
    }

    /// evaluate apply tree
    [[nodiscard]] inline auto eval_obj(object_ptr<const Object> obj)
      -> object_ptr<const Object>
    {
      return eval_spine(std::move(obj));
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
      return static_object_cast<To>(std::move(result));
    } else {
      // fallback to object_ptr<>
      return result;
    }
  }

} // namespace yave