//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/type_gen.hpp>
#include <yave/rts/apply.hpp>
#include <yave/rts/lambda.hpp>
#include <yave/rts/exception.hpp>
#include <yave/rts/type_error.hpp>
#include <yave/rts/utility.hpp>
#include <yave/rts/undefined.hpp>
#include <yave/rts/object_util.hpp>
#include <yave/rts/closure.hpp>

#include <vector>
#include <algorithm>

namespace yave {

  // fwd
  template <class T, class U>
  [[nodiscard]] auto value_cast_if(const object_ptr<U>& obj) noexcept
    -> object_ptr<propagate_const_t<T, U>>;
  template <class T, class U>
  [[nodiscard]] auto value_cast_if(object_ptr<U>&& obj) noexcept
    -> object_ptr<propagate_const_t<T, U>>;

  // ------------------------------------------
  // Utils

  /// type substitution
  struct type_arrow
  {
    object_ptr<const Type> from;
    object_ptr<const Type> to;
  };

  /// type constraint
  struct type_constr
  {
    object_ptr<const Type> t1;
    object_ptr<const Type> t2;
  };

  /// is_value_type
  [[nodiscard]] inline bool is_value_type(const object_ptr<const Type>& tp)
  {
    return get_if<value_type>(tp.value());
  }

  /// is_arrow_type
  [[nodiscard]] inline bool is_arrow_type(const object_ptr<const Type>& tp)
  {
    return get_if<arrow_type>(tp.value());
  }

  /// is_var_type
  [[nodiscard]] inline bool is_var_type(const object_ptr<const Type>& tp)
  {
    return get_if<var_type>(tp.value());
  }

  // is_list_type
  [[nodiscard]] inline bool is_list_type(const object_ptr<const Type>& tp)
  {
    return get_if<list_type>(tp.value());
  }

  /// has_value_type
  [[nodiscard]] inline bool has_value_type(const object_ptr<const Object>& obj)
  {
    return is_value_type(get_type(obj));
  }

  /// has_arrow_type
  [[nodiscard]] inline bool has_arrow_type(const object_ptr<const Object>& obj)
  {
    return is_arrow_type(get_type(obj));
  }

  /// has_var_type
  [[nodiscard]] inline bool has_var_type(const object_ptr<const Object>& obj)
  {
    return is_var_type(get_type(obj));
  }

  /// has_list_type
  [[nodiscard]] inline bool has_list_type(const object_ptr<const Object>& obj)
  {
    return is_list_type(get_type(obj));
  }

  // ------------------------------------------
  // flatten

  namespace detail {

    inline void flatten_impl(
      const object_ptr<const Type>& t,
      std::vector<object_ptr<const Type>>& v)
    {
      if (auto arr = get_if<arrow_type>(&*t)) {
        v.push_back(arr->captured);
        flatten_impl(arr->returns, v);
      } else {
        v.push_back(t);
      }
    }
  } // namespace detail

  /// flatten arrow type
  [[nodiscard]] inline auto flatten(const object_ptr<const Type>& tp)
    -> std::vector<object_ptr<const Type>>
  {
    assert(tp);
    std::vector<object_ptr<const Type>> ret;
    detail::flatten_impl(tp, ret);
    return ret;
  }

  // ------------------------------------------
  // copy_type

  namespace detail {

    inline auto copy_type_impl(const object_ptr<const Type>& t)
      -> object_ptr<const Type>
    {
      if (auto value = get_if<value_type>(t.value()))
        return make_object<Type>(value_type {*value});

      if (auto var = get_if<var_type>(t.value()))
        return make_object<Type>(var_type {var->id});

      if (auto arrow = get_if<arrow_type>(t.value()))
        return make_object<Type>(arrow_type {copy_type_impl(arrow->captured),
                                             copy_type_impl(arrow->returns)});

      if (auto list = get_if<list_type>(t.value()))
        return make_object<Type>(list_type {copy_type_impl(list->t)});

      unreachable();
    }
  } // namespace detail

  /// Deep copy type object
  [[nodiscard]] inline auto copy_type(const object_ptr<const Type>& tp)
    -> object_ptr<const Type>
  {
    assert(tp);
    return detail::copy_type_impl(tp);
  }

  // ------------------------------------------
  // same_type

  namespace detail {

    inline bool same_type_impl(
      const object_ptr<const Type>& left,
      const object_ptr<const Type>& right)
    {
      if (left.get() == right.get())
        return true;

      auto* l = left.value();
      auto* r = right.value();

      if (_get_storage(*l).index != _get_storage(*r).index)
        return false;

      if (auto lvar = get_if<value_type>(l))
        if (auto rvar = get_if<value_type>(r))
          return value_type::equal(*lvar, *rvar);

      if (auto larr = get_if<arrow_type>(l))
        if (auto rarr = get_if<arrow_type>(r))
          return same_type_impl(larr->captured, rarr->captured) &&
                 same_type_impl(larr->returns, rarr->returns);

      if (auto lany = get_if<var_type>(l))
        if (auto rany = get_if<var_type>(r))
          return lany->id == rany->id;

      if (auto llist = get_if<list_type>(l))
        if (auto rlist = get_if<list_type>(r))
          return same_type_impl(llist->t, rlist->t);

      unreachable();
    }
  } // namespace detail

  /// check type equality
  [[nodiscard]] inline bool same_type(
    const object_ptr<const Type>& lhs,
    const object_ptr<const Type>& rhs)
  {
    assert(lhs && rhs);
    return detail::same_type_impl(lhs, rhs);
  }

  // ------------------------------------------
  // subst_type

  namespace detail {

    // returns nullptr for identical subtree
    inline auto subst_impl_rec(
      const type_arrow& ta,
      const object_ptr<const Type>& in) -> object_ptr<const Type>
    {
      auto& from = ta.from;
      auto& to   = ta.to;

      if (auto arrow = get_if<arrow_type>(in.value())) {
        auto cap = subst_impl_rec(ta, arrow->captured);
        auto ret = subst_impl_rec(ta, arrow->returns);
        return (!cap && !ret)
                 ? nullptr
                 : make_object<Type>(arrow_type {cap ? cap : arrow->captured,
                                                 ret ? ret : arrow->returns});
      }

      if (auto list = get_if<list_type>(in.value())) {
        auto t = subst_impl_rec(ta, list->t);
        return !t ? nullptr : make_object<Type>(list_type {t});
      }

      if (same_type(in, from))
        return to;

      return nullptr;
    }

    inline auto subst_impl(
      const type_arrow& ta,
      const object_ptr<const Type>& in) -> object_ptr<const Type>
    {
      auto r = subst_impl_rec(ta, in);
      return r ? r : in;
    }

  } // namespace detail

  /// emulate type-substitution
  [[nodiscard]] inline auto subst_type(
    const type_arrow& ta,
    const object_ptr<const Type>& in) -> object_ptr<const Type>
  {
    return detail::subst_impl(ta, in);
  }

  // ------------------------------------------
  // subst_all

  /// apply all substitution
  [[nodiscard]] inline auto subst_type_all(
    const std::vector<type_arrow>& tyarrows,
    const object_ptr<const Type>& ty) -> object_ptr<const Type>
  {
    auto tmp = ty;
    for (auto ta : tyarrows) {
      tmp = subst_type(ta, tmp);
    }
    return tmp;
  }

  // ------------------------------------------
  // compose_subst

  /// compose substitution
  inline void compose_subst(
    std::vector<type_arrow>& tyarrows,
    const type_arrow& a)
  {
    for (auto&& ta : tyarrows) {
      ta.to = subst_type(a, ta.to);
    }

    for (auto&& ta : tyarrows) {
      if (same_type(ta.from, a.from))
        return;
    }
    tyarrows.push_back(a);
  }

  // ------------------------------------------
  // subst_constr

  /// subst_constr
  [[nodiscard]] inline auto subst_constr(
    const type_arrow& ta,
    const type_constr& constr) -> type_constr
  {
    return {subst_type(ta, constr.t1), subst_type(ta, constr.t2)};
  }

  // ------------------------------------------
  // subst_constr_all

  /// subst_constr_all
  [[nodiscard]] inline auto subst_constr_all(
    const type_arrow& ta,
    const std::vector<type_constr>& cs) -> std::vector<type_constr>
  {
    auto ret = std::vector<type_constr> {};
    ret.reserve(cs.size());

    for (auto&& c : cs)
      ret.push_back(subst_constr(ta, c));

    return ret;
  }

  // ------------------------------------------
  // occurs

  /// occurs
  [[nodiscard]] inline bool occurs(
    const object_ptr<const Type>& x,
    const object_ptr<const Type>& t)
  {
    if (get_if<value_type>(t.value()))
      return false;

    if (get_if<var_type>(t.value()))
      return same_type(x, t);

    if (auto arrow = get_if<arrow_type>(t.value()))
      return occurs(x, arrow->captured) || occurs(x, arrow->returns);

    if (auto list = get_if<list_type>(t.value()))
      return occurs(x, list->t);

    unreachable();
  }

  // ------------------------------------------
  // unify

  /// unify
  /// \param cs Type constraints
  /// \param src Source node (for error handling)
  [[nodiscard]] inline auto unify(std::vector<type_constr> constrs)
    -> std::vector<type_arrow>
  {
    auto cs = std::move(constrs);
    auto ta = std::vector<type_arrow> {};

    while (!cs.empty()) {
      auto c = cs.back();
      cs.pop_back();

      if (same_type(c.t1, c.t2))
        continue;

      if (is_var_type(c.t2)) {
        if (likely(!occurs(c.t2, c.t1))) {
          auto arr = type_arrow {c.t2, c.t1};
          cs       = subst_constr_all(arr, cs);
          compose_subst(ta, arr);
          continue;
        }
        throw type_error::circular_constraint(c.t1);
      }

      if (is_var_type(c.t1)) {
        if (likely(!occurs(c.t1, c.t2))) {
          auto arr = type_arrow {c.t1, c.t2};
          cs       = subst_constr_all(arr, cs);
          compose_subst(ta, arr);
          continue;
        }
        throw type_error::circular_constraint(c.t1);
      }

      if (is_arrow_type(c.t1) && is_arrow_type(c.t2)) {
        auto* arrow1 = get_if<arrow_type>(c.t1.value());
        auto* arrow2 = get_if<arrow_type>(c.t2.value());
        cs.push_back({arrow1->captured, arrow2->captured});
        cs.push_back({arrow1->returns, arrow2->returns});
        continue;
      }

      if (is_list_type(c.t1) && is_list_type(c.t2)) {
        auto list1 = get_if<list_type>(c.t1.value());
        auto list2 = get_if<list_type>(c.t2.value());
        cs.push_back({list1->t, list2->t});
        continue;
      }

      throw type_error::type_missmatch(c.t1, c.t2);
    }
    return ta;
  }

  // ------------------------------------------
  // genvar

  /// generate new type variable
  [[nodiscard]] inline auto genvar() -> object_ptr<const Type>
  {
    return make_object<Type>(var_type::random_generate());
  }

  // ------------------------------------------
  // vars

  namespace detail {

    inline void vars_impl(
      const object_ptr<const Type>& tp,
      std::vector<object_ptr<const Type>>& vars)
    {
      if (get_if<value_type>(tp.value()))
        return;

      if (get_if<var_type>(tp.value())) {
        for (auto&& v : vars) {
          if (same_type(v, tp))
            return;
        }
        vars.push_back(tp);
        return;
      }

      if (auto arrow = get_if<arrow_type>(tp.value())) {
        vars_impl(arrow->captured, vars);
        vars_impl(arrow->returns, vars);
        return;
      }

      if (auto list = get_if<list_type>(tp.value())) {
        vars_impl(list->t, vars);
        return;
      }

      unreachable();
    }
  } // namespace detail

  /// get list of type variables
  [[nodiscard]] inline auto vars(const object_ptr<const Type>& tp)
    -> std::vector<object_ptr<const Type>>
  {
    auto vars = std::vector<object_ptr<const Type>> {};
    detail::vars_impl(tp, vars);
    return vars;
  }

  // ------------------------------------------
  // genpoly

  /// create fresh polymorphic closure type
  [[nodiscard]] inline auto genpoly(
    const object_ptr<const Type>& tp,
    const std::vector<type_arrow>& env) -> object_ptr<const Type>
  {
    if (!is_arrow_type(tp))
      return tp;

    auto vs = vars(tp);
    auto t  = tp;

    for (auto v : vs) {

      if (std::find_if(env.begin(), env.end(), [&](auto&& a) {
            return same_type(a.from, v);
          }) != env.end())
        continue;

      auto a = type_arrow {v, genvar()};
      t      = subst_type(a, tp);
    }
    return t;
  }

  // ------------------------------------------
  // type_of

  namespace detail {

    // fwd
    inline auto type_of_impl(
      const object_ptr<const Object>& obj,
      std::vector<type_arrow>& env) -> object_ptr<const Type>;

    inline auto type_of_impl_app(
      const object_ptr<const Object>& obj,
      std::vector<type_arrow>& env) -> object_ptr<const Type>
    {
      return genpoly(type_of_impl(obj, env), env);
    }

    inline auto type_of_impl(
      const object_ptr<const Object>& obj,
      std::vector<type_arrow>& env) -> object_ptr<const Type>
    {
      // Apply
      if (auto apply = value_cast_if<Apply>(obj)) {

        auto& storage = _get_storage(*apply);

        // cached
        if (storage.is_result())
          return type_of_impl(storage.get_result(), env);

        auto t1 = type_of_impl_app(storage.app(), env);
        auto t2 = type_of_impl(storage.arg(), env);

        auto var = genvar();
        auto cs  = std::vector {type_constr {
          subst_type_all(env, t1), make_object<Type>(arrow_type {t2, var})}};
        auto as  = unify(std::move(cs));
        auto ty  = subst_type_all(as, var);

        auto end = std::remove_if(as.begin(), as.end(), [&](auto&& a) {
          return same_type(a.from, var);
        });
        as.erase(end, as.end());

        for (auto&& s : as)
          compose_subst(env, s);

        return ty;
      }

      // Lambda
      if (auto lambda = value_cast_if<Lambda>(obj)) {

        auto& storage = _get_storage(*lambda);

        auto var = make_object<Type>(var_type {storage.var->id()});
        env.push_back(type_arrow {var, var});

        auto t1 = type_of_impl(storage.var, env);
        auto t2 = type_of_impl(storage.body, env);

        auto ty = make_object<Type>(arrow_type {subst_type_all(env, t1), t2});

        auto end = std::remove_if(env.begin(), env.end(), [&](auto&& a) {
          return same_type(a.from, t1);
        });
        env.erase(end, env.end());

        return ty;
      }

      // Variable
      if (auto variable = value_cast_if<Variable>(obj)) {
        auto var = make_object<Type>(var_type {variable->id()});
        for (auto&& s : env) {
          if (same_type(s.from, var))
            return s.to;
        }
        throw type_error::unbounded_variable(var);
      }

      // arrow -> arrow or PAP
      if (has_arrow_type(obj)) {
        auto c = reinterpret_cast<const Closure<>*>(obj.get());
        // pap: return root apply node
        // app: get_type
        return c->is_pap() ? type_of_impl(c->vertebrae(c->arity), env)
                           : get_type(obj);
      }

      // value -> value
      if (has_value_type(obj))
        return get_type(obj);

      // var -> var
      if (has_var_type(obj))
        return get_type(obj);

      // list -> list
      if (has_list_type(obj))
        return get_type(obj);

      unreachable();
    }
  } // namespace detail

  /// type_of
  [[nodiscard]] inline auto type_of(const object_ptr<const Object>& obj)
    -> object_ptr<const Type>
  {
    std::vector<type_arrow> env;
    auto ty = detail::type_of_impl(obj, env);
    // FIXME: Is subst_type_all(env, ty) necessary?
    return ty;
  }

  // ------------------------------------------
  // has_type

  namespace detail {

    template <class T, class U>
    bool has_type_impl(const object_ptr<U>& obj)
    {
      // Apply
      if constexpr (std::is_same_v<std::decay_t<T>, Apply>) {
        return likely(obj) && _get_storage(obj).is_apply();
      }
      // Exception
      else if constexpr (std::is_same_v<std::decay_t<T>, Exception>) {
        return likely(obj) && _get_storage(obj).is_exception();
      }
      // Lambda
      else if constexpr (std::is_same_v<std::decay_t<T>, Lambda>) {
        return likely(obj) && _get_storage(obj).is_lambda();
      }
      // Variable
      else if constexpr (std::is_same_v<std::decay_t<T>, Variable>) {
        return likely(obj) && _get_storage(obj).is_variable();
      }
      // general
      else if constexpr (is_tm_value(get_term<T>())) {
        return same_type(get_type(obj), object_type<T>());
      } else
        static_assert(false_v<T>, "T is not value type");
    }

  } // namespace detail

  /// has_type
  template <class T, class U>
  [[nodiscard]] bool has_type(const object_ptr<U>& obj)
  {
    return detail::has_type_impl<T>(obj);
  }
} // namespace yave