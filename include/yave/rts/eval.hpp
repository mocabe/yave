//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/value_cast.hpp>
#include <yave/rts/closure.hpp>
#include <yave/rts/static_typing.hpp>
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

    // fwd
    inline auto eval_obj(const object_ptr<const Object>& obj)
      -> object_ptr<const Object>;

    /// Get spine depth and bottom closure
    inline auto inspect_spine(const object_ptr<const Object>& obj)
      -> std::pair<size_t, object_ptr<const Object>>
    {
      if (auto apply = value_cast_if<Apply>(obj)) {

        auto& storage = _get_storage(*apply);

        if (storage.is_result())
          return inspect_spine(storage.get_result());

        auto [depth, bottom] = inspect_spine(storage.app());
        return {depth + 1, std::move(bottom)};
      }
      return {0, obj};
    }

    /// Assign each vertebrae to spine stack
    /// \param obj apply tree
    /// \param stack mutable ref to stack buffer
    /// \param depth current depth of recursion
    inline void assign_spine_stack(
      const object_ptr<const Object>& obj,
      std::vector<object_ptr<const Apply>>& stack,
      const size_t& depth)
    {
      if (auto apply = value_cast_if<Apply>(obj)) {

        auto& storage = _get_storage(*apply);

        if (storage.is_result())
          return assign_spine_stack(storage.get_result(), stack, depth);

        assert(depth < stack.size());
        stack[depth] = std::move(apply);

        assign_spine_stack(storage.app(), stack, depth + 1);
      }
    }

    /// Assign each vertebrae to local stack
    /// \param obj apply tree
    /// \param closure bottom closure
    /// \param arity arity of bottom closure
    /// \param size size of local stack of bottom closure
    /// \param depth current depth of recursion
    inline void assign_spine_stack_direct(
      const object_ptr<const Object>& obj,
      const object_ptr<Closure<>>& closure,
      const size_t& arity,
      const size_t& size,
      const size_t& depth)
    {
      if (auto apply = value_cast_if<Apply>(obj)) {

        auto& storage = _get_storage(*apply);

        if (storage.is_result())
          return assign_spine_stack_direct(
            storage.get_result(), closure, arity, size, depth);

        assert(depth < size);
        closure->vertebrae(arity - size + depth) = std::move(apply);

        assign_spine_stack_direct(
          storage.app(), closure, arity, size, depth + 1);
      }
    }

    /// instantiate lambda body with actual argument
    inline auto instantiate_lambda_body(
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

    /// evaluete apply graph
    inline auto eval_spine(const object_ptr<const Object>& obj)
      -> object_ptr<const Object>
    {
      if (auto apply = value_cast_if<Apply>(obj)) {

        auto& apply_storage = _get_storage(*apply);

        // when we don't have any arguments to apply in stack and apply node
        // has already evaluated, we can directly return cached result.
        if (apply_storage.is_result())
          return apply_storage.get_result();

        // inspect spine structure without allocation
        auto [depth, bottom] = inspect_spine(apply_storage.app());

        // avoid allocation of stack buffer when spine stack can fit in local
        // stack of closure.
        if (auto closure = value_cast_if<Closure<>>(bottom)) {

          auto arity = closure->arity;
          auto size  = depth + 1;

          if (size <= arity) {

            auto fun = closure.clone();

            // dump to local stack directly
            assign_spine_stack_direct(apply_storage.app(), fun, arity, size, 1);
            fun->vertebrae(arity - size) = std::move(apply);
            fun->arity -= size;

            if (size != arity)
              return fun;

            auto result = fun->call();

            if (auto e = value_cast_if<Exception>(std::move(result)))
              throw exception_result(e);

            return result;
          }
        }

        // build spine stack on heap.
        // TODO: support allocating on stack when size of buffer is small
        // enough, but more than local stack size.
        auto stack = std::vector<object_ptr<const Apply>>(depth + 1);
        assign_spine_stack(apply_storage.app(), stack, 1);
        stack[0] = std::move(apply);

        for (;;) {

          // Handle lambda application
          // TODO: optimization
          if (auto lam = value_cast_if<Lambda>(bottom)) {
            // arg vartebrae
            auto& vert = _get_storage(*stack.back());
            // instantiate
            auto& lam_storage = _get_storage(*lam);
            auto inst         = instantiate_lambda_body(
              lam_storage.body, lam_storage.var, vert.arg());

            // eval body of lambda
            auto result = eval_obj(inst);

            assert(result);
            assert(!has_type<Exception>(result));

            // cache result
            vert.set_result(result);

            if (stack.size() == 1)
              return result;

            stack.pop_back();
            bottom = std::move(result);
            continue;
          }

          assert(value_cast_if<Closure<>>(bottom));

          // clone bottom closure
          auto fun   = bottom.clone();
          auto cfun  = reinterpret_cast<Closure<>*>(fun.get());
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
          if (auto e = value_cast_if<Exception>(std::move(result)))
            throw exception_result(e);

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
    [[nodiscard]] inline auto eval_obj(const object_ptr<const Object>& obj)
      -> object_ptr<const Object>
    {
      return eval_spine(obj);
    }

    template <class T>
    [[nodiscard]] inline auto eval_return(object_ptr<const Object> result)
    {
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
  } // namespace detail

  /// evaluate each apply node and replace with result
  template <class T>
  [[nodiscard]] auto eval(const object_ptr<T>& obj)
  {
    auto result = detail::eval_obj(obj);
    return detail::eval_return<T>(std::move(result));
  }

  /// evaluate each apply node and replace with result
  template <class T>
  [[nodiscard]] auto eval(object_ptr<T>&& obj)
  {
    auto result = detail::eval_obj(std::move(obj));
    return detail::eval_return<T>(std::move(result));
  }

} // namespace yave