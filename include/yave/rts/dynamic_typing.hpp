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

  /// \brief get **RAW** type of the object
  /// \notes return type of `Undefined` on null.
  /// \notes use type_of() to get actual type of terms.
  [[nodiscard]] inline object_ptr<const Type>
    get_type(const object_ptr<const Object>& obj)
  {
    if (obj)
      return _get_storage(obj).info_table()->obj_type;
    else
      return object_type<Undefined>();
  }

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

  [[nodiscard]] inline object_ptr<const Type>
    copy_type_impl(const object_ptr<const Type>& ptp)
  {
    if (auto value = get_if<value_type>(ptp.value()))
      return make_object<Type>(value_type {value->name});
    if (auto var = get_if<var_type>(ptp.value()))
      return make_object<Type>(var_type {var->id});
    if (auto arrow = get_if<arrow_type>(ptp.value())) {
      return make_object<Type>(arrow_type {copy_type_impl(arrow->captured),
                                           copy_type_impl(arrow->returns)});
    }

    unreachable();
  }

  /// Deep copy type object
  [[nodiscard]] inline object_ptr<const Type>
    copy_type(const object_ptr<const Type>& tp)
  {
    if (tp)
      return copy_type_impl(tp);
    else
      return tp;
  }

  namespace detail {
    [[nodiscard]] inline bool same_type_impl(
      const object_ptr<const Type>& lhs,
      const object_ptr<const Type>& rhs)
    {
      if (lhs.get() == rhs.get())
        return true;

      if (!lhs || !rhs)
        return false;

      const auto& left  = *lhs;
      const auto& right = *rhs;

      if (auto lvar = get_if<value_type>(&left)) {
        if (auto rvar = get_if<value_type>(&right))
          return value_type::compare(*lvar, *rvar);
        else
          return false;
      }
      if (auto larr = get_if<arrow_type>(&left)) {
        if (auto rarr = get_if<arrow_type>(&right))
          return same_type_impl(larr->captured, rarr->captured) &&
                 same_type_impl(larr->returns, rarr->returns);
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
  } // namespace detail

  /// check type equality
  [[nodiscard]] inline bool same_type(
    const object_ptr<const Type>& lhs,
    const object_ptr<const Type>& rhs)
  {
    return detail::same_type_impl(lhs, rhs);
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

  struct TyArrow
  {
    object_ptr<const Type> from;
    object_ptr<const Type> to;
  };

  namespace detail {
    [[nodiscard]] inline object_ptr<const Type>
      subst_type_impl(const TyArrow& ta, const object_ptr<const Type>& in)
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
          arrow_type {subst_type_impl(ta, arrow->captured),
                      subst_type_impl(ta, arrow->returns)});
      }

      unreachable();
    }
  } // namespace detail

  /// emulate type-substitution
  [[nodiscard]] inline object_ptr<const Type>
    subst_type(const TyArrow& ta, const object_ptr<const Type>& in)
  {
    return detail::subst_type_impl(ta, in);
  }

  /// apply all substitution
  [[nodiscard]] inline object_ptr<const Type> subst_type_all(
    const std::vector<TyArrow>& tyarrows,
    const object_ptr<const Type>& ty)
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

  // ------------------------------------------
  // Constr

  /// Type constraint
  struct Constr
  {
    object_ptr<const Type> t1;
    object_ptr<const Type> t2;
  };

  /// subst_constr
  [[nodiscard]] inline Constr
    subst_constr(const TyArrow& ta, const Constr& constr)
  {
    return {subst_type(ta, constr.t1), subst_type(ta, constr.t2)};
  }

  /// subst_constr_all
  [[nodiscard]] inline std::vector<Constr>
    subst_constr_all(const TyArrow& ta, const std::vector<Constr>& cs)
  {
    auto ret = std::vector<Constr> {};
    ret.reserve(cs.size());
    for (auto&& c : cs) ret.push_back(subst_constr(ta, c));
    return ret;
  }

  // ------------------------------------------
  // Occurs

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

  namespace detail {
    inline void unify_func_impl(
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
  } // namespace detail

  /// unify
  /// \param cs Type constraints
  /// \param src Source node (for error handling)
  [[nodiscard]] inline std::vector<TyArrow>
    unify(const std::vector<Constr>& cs, const object_ptr<const Object>& src)
  {
    auto _cs = cs;
    auto as  = std::vector<TyArrow> {};
    detail::unify_func_impl(_cs, as, src);
    return as;
  }

  // ------------------------------------------
  // Recon

  [[nodiscard]] inline object_ptr<const Type> genvar()
  {
    auto var                          = make_object<Type>(var_type {});
    get_if<var_type>(var.value())->id = uintptr_t(var.get());
    return object_ptr<const Type>(var);
  }

  namespace detail {
    inline void vars_impl(
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
        vars_impl(get_if<arrow_type>(tp.value())->captured, vars);
        vars_impl(get_if<arrow_type>(tp.value())->returns, vars);
        return;
      }

      unreachable();
    }
  } // namespace detail

  // get list of type variables
  [[nodiscard]] inline std::vector<object_ptr<const Type>>
    vars(const object_ptr<const Type>& tp)
  {
    auto vars = std::vector<object_ptr<const Type>> {};
    detail::vars_impl(tp, vars);
    return vars;
  }

  /// create fresh polymorphic closure type
  [[nodiscard]] inline object_ptr<const Type>
    genpoly(const object_ptr<const Type>& tp)
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

  namespace detail {
    // typing
    [[nodiscard]] inline const object_ptr<const Type>
      type_of_func_impl(const object_ptr<const Object>& obj)
    {
      // Apply
      if (auto apply = value_cast_if<const Apply>(obj)) {
        auto& apply_storage = _get_storage(*apply);
        auto _t1            = type_of_func_impl(apply_storage.app());
        auto _t2            = type_of_func_impl(apply_storage.arg());
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
        return genpoly(get_type(obj));

      unreachable();
    }
  } // namespace detail

  // type_of
  [[nodiscard]] inline object_ptr<const Type>
    type_of(const object_ptr<const Object>& obj)
  {
    return detail::type_of_func_impl(obj);
  }

} // namespace yave