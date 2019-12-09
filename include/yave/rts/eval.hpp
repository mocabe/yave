//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/value_cast.hpp>
#include <yave/rts/function.hpp>
#include <yave/rts/result_error.hpp>
#include <yave/rts/lambda.hpp>

#include <map>

namespace yave {

  /// copy apply graph
  [[nodiscard]] inline auto copy_apply_graph(
    const object_ptr<const Object>& root) -> object_ptr<const Object>
  {
    using map_type =
      std::map<object_ptr<const Object>, object_ptr<const Object>>;

    struct
    {
      auto rec(const object_ptr<const Object>& obj, map_type& map)
      {
        // find new apply node
        auto iter = map.find(obj);
        if (iter != map.end())
          return iter->second;

        if (auto apply = value_cast_if<Apply>(obj)) {

          auto& apply_storage = _get_storage(*apply);

          // return cached value
          if (apply_storage.is_result()) {
            return apply_storage.get_result();
          }

          // create new apply node
          object_ptr<const Object> new_app = make_object<Apply>(
            rec(apply_storage.app(), map), rec(apply_storage.arg(), map));

          map.emplace(std::move(apply), new_app);
          return new_app;
        }
        return obj;
      }
    } impl;

    map_type apply_map;
    return impl.rec(root, apply_map);
  }

  namespace detail {

    /// Build spine stack and assign spine pointers.
    /// Only allocates memory once at bottom, but uses more stack compared to
    /// incremental buffer allocation.
    [[nodiscard]] inline auto build_spine_stack(
      object_ptr<const Object> next,
      size_t depth) -> std::
      pair<object_ptr<const Object>, std::vector<object_ptr<const Apply>>>
    {
      if (auto apply = value_cast_if<Apply>(std::move(next))) {

        auto& apply_storage = _get_storage(*apply);

        if (apply_storage.is_result())
          return build_spine_stack(apply_storage.get_result(), depth);

        auto [bottom, stack] =
          build_spine_stack(apply_storage.app(), depth + 1);

        // assign each vertebrae
        stack[depth] = std::move(apply);

        return {std::move(bottom), std::move(stack)};
      }

      auto stack = std::vector<object_ptr<const Apply>>(depth);
      return {std::move(next), std::move(stack)};
    }

    /// instantiate lambda body with actual argument
    [[nodiscard]] inline auto instantiate_lambda_body(
      const object_ptr<const Object>& obj,
      const object_ptr<const Variable>& var,
      const object_ptr<const Object>& arg) -> object_ptr<const Object>
    {
      if (auto apply = value_cast_if<Apply>(obj)) {
        auto& storage = _get_storage(*apply);

        // assume result does not contain variable
        if (storage.is_result())
          return storage.get_result();

        return make_object<Apply>(
          instantiate_lambda_body(storage.app(), var, arg),
          instantiate_lambda_body(storage.arg(), var, arg));
      }

      if (auto lambda = value_cast_if<Lambda>(obj)) {
        auto& storage = _get_storage(*lambda);
        return make_object<Lambda>(
          storage.var, instantiate_lambda_body(storage.body, var, arg));
      }

      if (auto variable = value_cast_if<Variable>(obj)) {
        return (variable->id() == var->id()) ? arg : variable;
      }

      return obj;
    }

    [[nodiscard]] inline auto eval_spine(object_ptr<const Object> obj)
      -> object_ptr<const Object>
    {
      // exploit value_cast_if does not modify rvalue ref on fail
      if (auto apply = value_cast_if<Apply>(std::move(obj))) {

        auto& apply_storage = _get_storage(*apply);

        // when we don't have any arguments to apply in stack and apply node
        // has already evaluated, we can directly return cached result. if we
        // can guarantee cached results are only PAP or values, we actually
        // don't need to call eval_spine() on these results.
        if (apply_storage.is_result())
          return apply_storage.get_result();

        // build stack and get bottom closure/lambda
        auto [bottom, stack] = build_spine_stack(apply_storage.app(), 1);
        stack[0]             = std::move(apply);

        for (;;) {

          // Handle lambda application
          // TODO: optimization
          if (auto lam = value_cast_if<Lambda>(bottom)) {
            // get next arg
            auto& vert = _get_storage(*stack.back());
            auto arg   = vert.arg();
            // instantiate
            auto& storage = _get_storage(*lam);
            auto inst     = instantiate_lambda_body(
              storage.body, storage.var, std::move(arg));

            // eval body of lambda
            auto result = eval_obj(inst);
            vert.set_result(result);

            if (stack.size() == 1)
              return result;

            stack.pop_back();
            bottom = std::move(result);
            continue;
          }

          assert(has_arrow_type(bottom));

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

          // detect exception
          if (auto excpt = value_cast_if<Exception>(result))
            throw exception_result(excpt);

          // completed
          if (stack.size() == arity)
            return result;

          // unwind stack consumed
          stack.erase(base_iter, stack.end());

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
    if constexpr (!is_tyerror(type)) {
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