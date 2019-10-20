//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/rts/type_gen.hpp>
#include <yave/rts/apply.hpp>
#include <yave/rts/exception.hpp>
#include <yave/rts/type_error.hpp>
#include <yave/rts/utility.hpp>
#include <yave/rts/undefined.hpp>
#include <yave/rts/object_util.hpp>
#include <yave/rts/closure.hpp>

#include <vector>

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

  /// is_value_type
  [[nodiscard]] inline bool is_value_type(const object_ptr<const Type>& tp)
  {
    if (get_if<value_type>(tp.value()))
      return true;
    else
      return false;
  }

  /// is_arrow_type
  [[nodiscard]] inline bool is_arrow_type(const object_ptr<const Type>& tp)
  {
    if (get_if<arrow_type>(tp.value()))
      return true;
    else
      return false;
  }

  /// is_var_type
  [[nodiscard]] inline bool is_var_type(const object_ptr<const Type>& tp)
  {
    if (get_if<var_type>(tp.value()))
      return true;
    else
      return false;
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

  /// flatten arrow type (non recursive)
  [[nodiscard]] inline auto flatten(const object_ptr<const Type>& tp)
    -> std::vector<object_ptr<const Type>>
  {
    struct
    {
      void rec(
        const object_ptr<const Type>& t,
        std::vector<object_ptr<const Type>>& v)
      {
        if (!t)
          return;
        if (auto arr = get_if<arrow_type>(&*t)) {
          v.push_back(arr->captured);
          rec(arr->returns, v);
        } else
          v.push_back(t);
      }
    } impl;

    std::vector<object_ptr<const Type>> ret;
    impl.rec(tp, ret);
    return ret;
  }

  struct type_arrow
  {
    object_ptr<const Type> from;
    object_ptr<const Type> to;
  };

  struct type_constr
  {
    object_ptr<const Type> t1;
    object_ptr<const Type> t2;
  };

  /// Deep copy type object
  [[nodiscard]] inline auto copy_type(const object_ptr<const Type>& tp)
    -> object_ptr<const Type>
  {
    struct
    {
      auto rec(const object_ptr<const Type>& t) -> object_ptr<const Type>
      {
        if (auto value = get_if<value_type>(t.value()))
          return make_object<Type>(value_type {*value});

        if (auto var = get_if<var_type>(t.value()))
          return make_object<Type>(var_type {var->id});

        if (auto arrow = get_if<arrow_type>(t.value())) {
          return make_object<Type>(
            arrow_type {rec(arrow->captured), rec(arrow->returns)});
        }
        unreachable();
      }
    } impl;

    if (!tp)
      return tp;

    return impl.rec(tp);
  }

  /// check type equality
  [[nodiscard]] inline bool same_type(
    const object_ptr<const Type>& lhs,
    const object_ptr<const Type>& rhs)
  {
    struct
    {
      auto rec(const object_ptr<const Type>& l, const object_ptr<const Type>& r)
        -> bool
      {
        if (l.get() == r.get())
          return true;

        if (!l || !r)
          return false;

        const auto& left  = *l;
        const auto& right = *r;

        if (auto lvar = get_if<value_type>(&left)) {
          if (auto rvar = get_if<value_type>(&right))
            return value_type::equal(*lvar, *rvar);
          else
            return false;
        }

        if (auto larr = get_if<arrow_type>(&left)) {
          if (auto rarr = get_if<arrow_type>(&right))
            return rec(larr->captured, rarr->captured) &&
                   rec(larr->returns, rarr->returns);
          else
            return false;
        }

        if (auto lany = get_if<var_type>(&left)) {
          if (auto rany = get_if<var_type>(&right))
            return lany->id == rany->id;
          else
            return false;
        }
        unreachable();
      }
    } impl;

    return impl.rec(lhs, rhs);
  }

  /// emulate type-substitution
  [[nodiscard]] inline auto subst_type(
    const type_arrow& ta,
    const object_ptr<const Type>& in) -> object_ptr<const Type>
  {
    struct
    {
      auto rec(const type_arrow& ta, const object_ptr<const Type>& in)
        -> object_ptr<const Type>
      {
        auto& from = ta.from;
        auto& to   = ta.to;

        if (get_if<value_type>(in.value())) {
          if (same_type(in, from))
            return to;
          return in;
        }

        if (get_if<var_type>(in.value())) {
          if (same_type(in, from))
            return to;
          return in;
        }

        if (auto arrow = get_if<arrow_type>(in.value())) {
          if (same_type(in, from))
            return to;
          return make_object<Type>(
            arrow_type {rec(ta, arrow->captured), rec(ta, arrow->returns)});
        }
        unreachable();
      }
    } impl;

    return impl.rec(ta, in);
  }

  /// apply all substitution
  [[nodiscard]] inline auto subst_type_all(
    const std::vector<type_arrow>& tyarrows,
    const object_ptr<const Type>& ty) -> object_ptr<const Type>
  {
    auto t = ty;
    for (auto ta : tyarrows) {
      t = subst_type(ta, t);
    }
    return t;
  }

  /// compose substitution
  inline void compose_subst(
    std::vector<type_arrow>& tyarrows,
    const type_arrow& a)
  {
    for (auto&& ta : tyarrows) {
      ta.to = subst_type(a, ta.to);
    }

    [&]() {
      for (auto&& ta : tyarrows) {
        if (same_type(ta.from, a.from))
          return;
      }
      tyarrows.push_back(a);
    }();
  }

  /// subst_constr
  [[nodiscard]] inline auto subst_constr(
    const type_arrow& ta,
    const type_constr& constr) -> type_constr
  {
    return {subst_type(ta, constr.t1), subst_type(ta, constr.t2)};
  }

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

    unreachable();
  }

  /// unify
  /// \param cs Type constraints
  /// \param src Source node (for error handling)
  [[nodiscard]] inline auto unify(
    const std::vector<type_constr>& constrs,
    const object_ptr<const Object>& src) -> std::vector<type_arrow>
  {
    auto cs = constrs;
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
        throw type_error::circular_constraint(src, c.t1);
      }
      if (is_var_type(c.t1)) {
        if (likely(!occurs(c.t1, c.t2))) {
          auto arr = type_arrow {c.t1, c.t2};
          cs       = subst_constr_all(arr, cs);
          compose_subst(ta, arr);
          continue;
        }
        throw type_error::circular_constraint(src, c.t1);
      }
      if (auto arrow1 = get_if<arrow_type>(c.t1.value())) {
        if (auto arrow2 = get_if<arrow_type>(c.t2.value())) {
          cs.push_back({arrow1->captured, arrow2->captured});
          cs.push_back({arrow1->returns, arrow2->returns});
          continue;
        }
      }
      throw type_error::type_missmatch(src, c.t1, c.t2);
    }

    return ta;
  }

  /// generate new type variable
  [[nodiscard]] inline auto genvar() -> object_ptr<const Type>
  {
    auto var = make_object<Type>(var_type::random_generate());
    return object_ptr<const Type>(var);
  }

  /// get list of type variables
  [[nodiscard]] inline auto vars(const object_ptr<const Type>& tp)
    -> std::vector<object_ptr<const Type>>
  {
    struct
    {
      void rec(
        const object_ptr<const Type>& tp,
        std::vector<object_ptr<const Type>>& vars)
      {
        if (is_value_type(tp))
          return;
        if (is_var_type(tp)) {
          return [&]() {
            for (auto&& v : vars) {
              if (same_type(v, tp))
                return;
            }
            vars.push_back(tp);
          }();
        }
        if (is_arrow_type(tp)) {
          rec(get_if<arrow_type>(tp.value())->captured, vars);
          rec(get_if<arrow_type>(tp.value())->returns, vars);
          return;
        }

        unreachable();
      }
    } impl;

    auto vars = std::vector<object_ptr<const Type>> {};
    impl.rec(tp, vars);
    return vars;
  }

  /// create fresh polymorphic closure type
  [[nodiscard]] inline auto genpoly(const object_ptr<const Type>& tp)
    -> object_ptr<const Type>
  {
    if (!is_arrow_type(tp))
      return tp;
    auto vs = vars(tp);
    auto t  = tp;
    for (auto v : vs) {
      auto a = type_arrow {v, genvar()};
      t      = subst_type(a, tp);
    }
    return t;
  }

  /// type_of
  [[nodiscard]] inline auto type_of(const object_ptr<const Object>& obj)
    -> object_ptr<const Type>
  {
    struct
    {
      auto rec_app(const object_ptr<const Object>& obj)
        -> const object_ptr<const Type>
      {
        if (has_arrow_type(obj))
          return genpoly(get_type(obj));
        else
          return rec(obj);
      }

      auto rec(const object_ptr<const Object>& obj)
        -> const object_ptr<const Type>
      {
        // Apply
        if (auto apply = value_cast_if<const Apply>(obj)) {
          auto& apply_storage = _get_storage(*apply);
          // cached
          if (apply_storage.is_result()) {
            return rec(apply_storage.get_result());
          }
          auto _t1 = rec_app(apply_storage.app());
          auto _t2 = rec(apply_storage.arg());
          auto _t  = genvar();
          auto c   = std::vector {
            type_constr {_t1, make_object<Type>(arrow_type {_t2, _t})}};
          auto s = unify(std::move(c), obj);
          return subst_type_all(s, _t);
        }
        // value -> value
        if (has_value_type(obj))
          return get_type(obj);
        // var -> var
        if (has_var_type(obj))
          return get_type(obj);
        // arrow -> arrow or PAP
        if (has_arrow_type(obj)) {
          auto c = reinterpret_cast<const Closure<>*>(obj.get());
          // pap: return root apply node
          if (c->is_pap()) {
            return rec(c->vertebrae(0));
          }
          // arrow: get_type
          return get_type(obj);
        }

        unreachable();
      }
    } impl;

    return impl.rec(obj);
  }

  /// has_type
  template <class T, class U>
  [[nodiscard]] bool has_type(const object_ptr<U>& obj)
  {
    if (same_type(get_type(obj), object_type<T>()))
      return true;
    else
      return false;
  }

} // namespace yave