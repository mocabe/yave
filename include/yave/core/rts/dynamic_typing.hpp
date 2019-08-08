//
// Copyright (c) 2019 mocabe (https://github.com/mocabe)
// Distributed under LGPLv3 License. See LICENSE for more details.
//

#pragma once

#include <yave/core/rts/type_gen.hpp>
#include <yave/core/rts/apply.hpp>
#include <yave/core/rts/exception.hpp>
#include <yave/core/rts/type_error.hpp>
#include <yave/core/rts/utility.hpp>
#include <yave/core/rts/undefined.hpp>
#include <yave/core/rts/object_util.hpp>

#include <vector>

namespace yave {

  // fwd
  template <class T, class U>
  [[nodiscard]] object_ptr<propagate_const_t<T, U>>
    value_cast_if(const object_ptr<U>& obj) noexcept;
  template <class T, class U>
  [[nodiscard]] object_ptr<propagate_const_t<T, U>>
    value_cast_if(object_ptr<U>&& obj) noexcept;

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
    } _impl;

    std::vector<object_ptr<const Type>> ret;
    _impl.rec(tp, ret);
    return ret;
  }

  struct TyArrow
  {
    object_ptr<const Type> from;
    object_ptr<const Type> to;
  };

  struct Constr
  {
    object_ptr<const Type> t1;
    object_ptr<const Type> t2;
  };

  /// Deep copy type object
  [[nodiscard]] inline auto copy_type(const object_ptr<const Type>& tp)
    -> object_ptr<const Type>
  {
    if (!tp)
      return tp;

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
    } _impl;

    return _impl.rec(tp);
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
    } _impl;

    return _impl.rec(lhs, rhs);
  }

  /// emulate type-substitution
  [[nodiscard]] inline auto
    subst_type(const TyArrow& ta, const object_ptr<const Type>& in)
      -> object_ptr<const Type>
  {
    struct
    {
      auto rec(const TyArrow& ta, const object_ptr<const Type>& in)
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
    } _impl;

    return _impl.rec(ta, in);
  }

  /// apply all substitution
  [[nodiscard]] inline auto subst_type_all(
    const std::vector<TyArrow>& tyarrows,
    const object_ptr<const Type>& ty) -> object_ptr<const Type>
  {
    auto t = ty;
    for (auto ta : tyarrows) {
      t = subst_type(ta, t);
    }
    return t;
  }

  /// compose substitution
  inline void compose_subst(std::vector<TyArrow>& tyarrows, const TyArrow& a)
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
  [[nodiscard]] inline auto
    subst_constr(const TyArrow& ta, const Constr& constr) -> Constr
  {
    return {subst_type(ta, constr.t1), subst_type(ta, constr.t2)};
  }

  /// subst_constr_all
  [[nodiscard]] inline auto
    subst_constr_all(const TyArrow& ta, const std::vector<Constr>& cs)
      -> std::vector<Constr>
  {
    auto ret = std::vector<Constr> {};
    ret.reserve(cs.size());
    for (auto&& c : cs) ret.push_back(subst_constr(ta, c));
    return ret;
  }

  /// occurs
  [[nodiscard]] inline bool
    occurs(const object_ptr<const Type>& x, const object_ptr<const Type>& t)
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
  [[nodiscard]] inline auto
    unify(const std::vector<Constr>& cs, const object_ptr<const Object>& src)
      -> std::vector<TyArrow>
  {
    struct
    {
      void operator()(
        std::vector<Constr>& cs,
        std::vector<TyArrow>& ta,
        const object_ptr<const Object>& src)
      {
        while (!cs.empty()) {
          auto c = cs.back();
          cs.pop_back();
          if (same_type(c.t1, c.t2))
            continue;
          if (is_var_type(c.t2)) {
            if (likely(!occurs(c.t2, c.t1))) {
              auto arr = TyArrow {c.t2, c.t1};
              cs       = subst_constr_all(arr, cs);
              compose_subst(ta, arr);
              continue;
            }
            throw type_error::circular_constraint(src, c.t1);
          }
          if (is_var_type(c.t1)) {
            if (likely(!occurs(c.t1, c.t2))) {
              auto arr = TyArrow {c.t1, c.t2};
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
      }
    } _impl;

    auto _cs = cs;
    auto as  = std::vector<TyArrow> {};
    _impl(_cs, as, src);
    return as;
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
    } _impl;

    auto vars = std::vector<object_ptr<const Type>> {};
    _impl.rec(tp, vars);
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
      auto a = TyArrow {v, genvar()};
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
          auto _t1            = rec_app(apply_storage.app());
          auto _t2            = rec(apply_storage.arg());
          auto _t             = genvar();
          auto c =
            std::vector {Constr {_t1, make_object<Type>(arrow_type {_t2, _t})}};
          auto s = unify(std::move(c), obj);
          return subst_type_all(s, _t);
        }
        // value -> value
        if (has_value_type(obj))
          return get_type(obj);
        // var -> var
        if (has_var_type(obj))
          return get_type(obj);
        // arrow -> genpoly arrow
        if (has_arrow_type(obj))
          return get_type(obj);

        unreachable();
      }
    } _impl;

    return _impl.rec(obj);
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